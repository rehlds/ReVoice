# Set the main version numbers
set(CMAKE_PROJECT_VERSION_MAJOR 1)
set(CMAKE_PROJECT_VERSION_MINOR 0)
set(SVN_OLD_NUMBER 664) # Base commit count offset

# Search for Git on the system
find_package(Git QUIET)

if(GIT_FOUND)
  # Get the current branch name
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Get the total number of commits in the current branch
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" rev-list --count ${GIT_BRANCH}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_COMMIT_COUNT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Add the base commit count offset to the total commit count
  math(EXPR GIT_COMMIT_COUNT "${GIT_COMMIT_COUNT} + ${SVN_OLD_NUMBER}")

  # Get the full SHA hash of the current commit
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" rev-parse --verify HEAD
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_SHA_FULL
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Extract the first 7 characters of the SHA
  string(SUBSTRING "${GIT_SHA_FULL}" 0 7 GIT_COMMIT_SHA)

  # Get the date of the last commit
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" log -1 --format=%cd --date=format:%Y-%m-%d
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_COMMIT_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Get the time of the last commit
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" log -1 --format=%cd --date=format:%H:%M:%S
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_COMMIT_TIME
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Check if there are any local modifications (uncommitted changes)
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" ls-files -m
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_MODIFIED
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # If there are modified files, mark the version as modified
  if(GIT_MODIFIED)
    set(VERSION_MODIFIED "+m")
  else()
    set(VERSION_MODIFIED "")
  endif()

  # Get the repository's remote URL
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" config --get remote.origin.url
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_URL
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Remove the ".git" suffix from the URL if it exists
  string(REGEX REPLACE "\\.git$" "" GIT_URL "${GIT_URL}")

  # Format the commit URL based on the hosting platform
  if(GIT_URL MATCHES "bitbucket.org")
    set(GIT_COMMIT_URL "${GIT_URL}/commits/")
  else()
    set(GIT_COMMIT_URL "${GIT_URL}/commit/")
  endif()
else()
  message(WARNING "Git not found, auto-versioning disabled")
  set(GIT_COMMIT_COUNT 0)
  set(GIT_COMMIT_SHA "unknown")
  set(VERSION_MODIFIED "")
  set(GIT_COMMIT_URL "unknown")
  set(GIT_COMMIT_DATE "unknown")
  set(GIT_COMMIT_TIME "unknown")
endif()

# Format the project version
set(CMAKE_PROJECT_VERSION_PATCH "${GIT_COMMIT_COUNT}")
set(CMAKE_PROJECT_VERSION_TWEAK "0")
set(CMAKE_PROJECT_VERSION
  "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}${VERSION_MODIFIED}"
)
