#include "precompiled.h"

VoiceEncoder_Silk::VoiceEncoder_Silk()
{
	m_pEncoder = nullptr;
	m_pDecoder = nullptr;
	m_targetRate_bps = 25000;
	m_packetLoss_perc = 0;
}

VoiceEncoder_Silk::~VoiceEncoder_Silk()
{
	if (m_pEncoder) {
		free(m_pEncoder);
		m_pEncoder = nullptr;
	}

	if (m_pDecoder) {
		free(m_pDecoder);
		m_pDecoder = nullptr;
	}
}

bool VoiceEncoder_Silk::Init(int quality)
{
	m_targetRate_bps = 25000;
	m_packetLoss_perc = 0;

	int encSizeBytes;
	SKP_Silk_SDK_Get_Encoder_Size(&encSizeBytes);
	m_pEncoder = malloc(encSizeBytes);
	SKP_Silk_SDK_InitEncoder(m_pEncoder, &this->m_encControl);

	int decSizeBytes;
	SKP_Silk_SDK_Get_Decoder_Size(&decSizeBytes);
	m_pDecoder = malloc(decSizeBytes);
	SKP_Silk_SDK_InitDecoder(m_pDecoder);

	return true;
}

void VoiceEncoder_Silk::Release()
{
	delete this;
}

bool VoiceEncoder_Silk::ResetState()
{
	if (m_pEncoder)
		SKP_Silk_SDK_InitEncoder(m_pEncoder, &this->m_encControl);

	if (m_pDecoder)
		SKP_Silk_SDK_InitDecoder(m_pDecoder);

	m_bufOverflowBytes.Clear();

	return true;
}

int VoiceEncoder_Silk::Compress(const char *pUncompressedIn, int nSamplesIn, char *pCompressed, int maxCompressedBytes, bool bFinal)
{
	const int inSampleRate = 8000;
	const int nSampleDataMinMS = 20;
	const int nSamplesMin = (nSampleDataMinMS * inSampleRate) / 1000;

	if (nSamplesIn + GetNumQueuedEncodingSamples() < nSamplesMin && !bFinal)
	{
		m_bufOverflowBytes.Put(pUncompressedIn, nSamplesIn * BYTES_PER_SAMPLE);
		return 0;
	}

	const int16_t *psRead = (const int16_t *)pUncompressedIn;

	int nSamplesToUse = nSamplesIn;
	int nSamplesPerFrame = nSamplesMin;
	int nSamplesRemaining = nSamplesIn % nSamplesMin;

	if (m_bufOverflowBytes.TellPut() || nSamplesRemaining && bFinal)
	{
		m_bufOverflowBytes.Put(pUncompressedIn, nSamplesIn * BYTES_PER_SAMPLE);
		nSamplesToUse = GetNumQueuedEncodingSamples();
		nSamplesRemaining = nSamplesToUse % nSamplesPerFrame;

		if (bFinal && nSamplesRemaining)
		{
			// fill samples of silence at the remaining bytes
			for (int i = nSamplesPerFrame - nSamplesRemaining; i > 0; i--)
			{
				m_bufOverflowBytes.PutShort(0);
			}

			nSamplesToUse = GetNumQueuedEncodingSamples();
			nSamplesRemaining = nSamplesToUse % nSamplesPerFrame;
		}

		psRead = (const int16_t *)m_bufOverflowBytes.Base();
		Assert(!bFinal || nSamplesRemaining == 0);
	}

	char *pWritePos = pCompressed;
	const char *pWritePosMax = &pCompressed[maxCompressedBytes];

	int nSamples = nSamplesToUse - nSamplesRemaining;
	while (nSamples > 0)
	{
		int16_t *pWritePayloadSize = (int16_t *)pWritePos;
		pWritePos += sizeof(int16_t);

		m_encControl.maxInternalSampleRate = 24000;
		m_encControl.useInBandFEC = 0;
		m_encControl.useDTX = 1;
		m_encControl.complexity = 2;
		m_encControl.API_sampleRate = inSampleRate;
		m_encControl.packetSize = 20 * (inSampleRate / 1000);
		m_encControl.packetLossPercentage = m_packetLoss_perc;
		m_encControl.bitRate = 30000;//(m_targetRate_bps >= 0) ? m_targetRate_bps : 0;//clamp(voice_silk_bitrate.GetInt(), 10000, 40000);

		int nSamplesToEncode = min(nSamples, nSamplesPerFrame);

		int nBytes = ((pWritePosMax - pWritePos) < 0x7FFF) ? (pWritePosMax - pWritePos) : 0x7FFF;
		int ret = SKP_Silk_SDK_Encode(m_pEncoder, &m_encControl, psRead, nSamplesToEncode, (unsigned char *)pWritePos, (__int16 *)&nBytes);
		*pWritePayloadSize = nBytes; // write frame size

		nSamples -= nSamplesToEncode;
		psRead += nSamplesToEncode;
		pWritePos += nBytes;
	}

	m_bufOverflowBytes.Clear();

	if (nSamplesRemaining && nSamplesRemaining <= nSamplesIn)
	{
		m_bufOverflowBytes.Put(pUncompressedIn + ((nSamplesIn - nSamplesRemaining) * sizeof(int16_t)), nSamplesRemaining * BYTES_PER_SAMPLE);
	}

	if (bFinal)
	{
		ResetState();

		if (pWritePosMax > pWritePos + 2)
		{
			uint16_t *pWriteEndFlag = (uint16_t *)pWritePos;
			pWritePos += sizeof(uint16_t);
			*pWriteEndFlag = 0xFFFF;
		}
	}

	return pWritePos - pCompressed;
}

int VoiceEncoder_Silk::Decompress(const char *pCompressed, int compressedBytes, char *pUncompressed, int maxUncompressedBytes)
{
	int nPayloadSize; // ebp@2
	char *pWritePos; // ebx@5
	const char *pReadPos; // edx@5
	char *pWritePosMax; // [sp+28h] [bp-44h]@4
	const char *pReadPosMax; // [sp+3Ch] [bp-30h]@1

	const int outSampleRate = 8000;

	m_decControl.API_sampleRate = outSampleRate;
	int nSamplesPerFrame = outSampleRate / 50;

	if (compressedBytes <= 0) {
		return 0;
	}

	pReadPosMax = &pCompressed[compressedBytes];
	pReadPos = pCompressed;

	pWritePos = pUncompressed;
	pWritePosMax = &pUncompressed[maxUncompressedBytes];

	while (pReadPos < pReadPosMax)
	{
		if (pReadPosMax - pReadPos < 2) {
			break;
		}

		nPayloadSize = *(uint16 *)pReadPos;
		pReadPos += sizeof(uint16);

		if (nPayloadSize == 0xFFFF) {
			ResetState();
			break;
		}

		if (nPayloadSize == 0) {
			//DTX (discontinued transmission)
			int numEmptySamples = nSamplesPerFrame;
			short nSamples = (pWritePosMax - pWritePos) / 2;

			if (nSamples < numEmptySamples) {
				break;
			}

			memset(pWritePos, 0, numEmptySamples * 2);
			pWritePos += numEmptySamples * 2;

			continue;
		}

		if ((pReadPos + nPayloadSize) > pReadPosMax) {
			break;
		}

		do {
			short nSamples = (pWritePosMax - pWritePos) / 2;
			int decodeRes = SKP_Silk_SDK_Decode(m_pDecoder, &m_decControl, 0, (const unsigned char*)pReadPos, nPayloadSize, (__int16 *)pWritePos, &nSamples);

			if (SKP_SILK_NO_ERROR != decodeRes) {
				return (pWritePos - pUncompressed) / 2;
			}

			pWritePos += nSamples * sizeof(int16);
		} while (m_decControl.moreInternalDecoderFrames);
		pReadPos += nPayloadSize;
	}

	return (pWritePos - pUncompressed) / 2;
}