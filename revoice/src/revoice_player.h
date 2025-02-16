#pragma once

#include "revoice_shared.h"
#include "VoiceEncoder_Silk.h"
#include "SteamP2PCodec.h"
#include "VoiceEncoder_Speex.h"
#include "voice_codec_frame.h"

class CRevoicePlayer {
private:
	IGameClient *m_Client;
	CodecType m_CodecType;
	CSteamP2PCodec *m_SilkCodec;
	CSteamP2PCodec *m_OpusCodec;
	VoiceCodec_Frame *m_SpeexCodec;
	int m_Protocol;
	int m_VoiceRate;
	int m_RequestId;
	bool m_Connected;
	bool m_HLTV;

public:
	CRevoicePlayer();
	void Update();
	void Initialize(IGameClient *cl);
	void OnConnected();
	void OnDisconnected();

	void SetLastVoiceTime(double time);
	void UpdateVoiceRate(double delta);
	void IncreaseVoiceRate(int dataLength);
	CodecType GetCodecTypeByString(const char *codec);
	const char *GetCodecTypeToString();

	int GetProtocol()  const { return m_Protocol;  }
	int GetVoiceRate() const { return m_VoiceRate; }
	int GetRequestId() const { return m_RequestId; }
	bool IsConnected() const { return m_Connected; }
	bool IsHLTV()      const { return m_HLTV;      }

	static const char *m_szCodecType[];
	void SetCodecType(CodecType codecType)     { m_CodecType = codecType; }

	CodecType GetCodecType() const             { return m_CodecType; }
	CSteamP2PCodec *GetSilkCodec() const       { return m_SilkCodec; }
	CSteamP2PCodec *GetOpusCodec() const       { return m_OpusCodec; }
	VoiceCodec_Frame *GetSpeexCodec() const    { return m_SpeexCodec;  }
	IGameClient *GetClient() const             { return m_Client; }
};

extern CRevoicePlayer g_Players[MAX_PLAYERS];

CRevoicePlayer *GetPlayerByClientPtr(IGameClient *cl);
CRevoicePlayer *GetPlayerByEdict(const edict_t *ed);

void Revoice_Init_Players();
void Revoice_Update_Players(const char *pszNewValue);
void Revoice_Update_Hltv(const char *pszNewValue);
