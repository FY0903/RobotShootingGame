/*+===================================================================
	File: Sound.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/08 11:56:17 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <mmreg.h>
#include <MSAcm.h>
#include <Shlwapi.h>

// ==============================
//	lib
// ==============================
#pragma comment(lib, "msacm32.lib")
#pragma comment(lib, "shlwapi.lib")

#undef PlaySound // winapiのPlaySoundを無効にする

/**
 * @brief Soundクラス
 */
class Sound : public Singleton<Sound>
{
public:
	HRESULT Init();
	void Uninit();

	XAUDIO2_BUFFER* Load(const std::string& In_strSoundName, const std::string& In_strFileName,
		const bool& In_bIsSE = true, const bool& In_bLoop = false);

	void Play(const std::string& In_strSoundName);

	bool IsPlaying(const std::string& In_strSoundName) const;

	void Stop(const std::string& In_strSoundName, bool In_bIsFlag = false);

	void SetVolume(const std::string& In_strSoundName, float In_nVolume);

	void UnLoad(const std::string& In_strSoundName);

	inline IXAudio2* GetXAudio() const { return m_pXAudio.Get(); }

	inline const float& GetBGMVolume() const { return m_fBGMVolume; }

	inline const float& GetSEVolume() const { return m_fSEVolume; }

	void SetSEVolume(const float& In_nVolume);

	void SetBGMVolume(const float& In_nVolume);

private:
	/**
	 * @brief サウンドデータを格納するための構造体です。
	 */
	struct Data
	{
		std::string strFilePath;			//ファイルパス
		WAVEFORMATEX	format{};			// WAVフォーマット
		std::shared_ptr<BYTE> pBuffer{};	// サウンドデータ
		DWORD			bufSize{};			// データサイズ
		XAUDIO2_BUFFER	sound{};			// サウンドバッファ
	};

	/**
	 * @brief サウンドデータを表す構造体です。
	 */
	struct SoundData
	{
		Data data{};
		IXAudio2SourceVoice* pSourceVoice{};
		float fVolume{};
		bool bIsSE{};
	};

	/**
	 * @brief MP3データのオフセットとサイズ情報を保持する構造体です。
	 */
	struct MP3FormatInfo
	{
		DWORD offset{};
		DWORD dataSize{};
	};

	/**
	 * @brief MP3フレーム情報を保持する構造体です。
	 */
	struct MP3FrameInfo
	{
		BYTE channel{};
		BYTE padding{};
		DWORD sampleRate{};
		DWORD bitRate{};
		DWORD frameSize{};
	};

	friend class Singleton<Sound>;

	/**
	 * コンストラクタ
	 */
	Sound();

	/**
	 * デストラクタ
	 */
	~Sound();

	HRESULT LoadWav(const char* In_c_cpFile, SoundData* In_pData);

	HRESULT LoadMP3(const char* In_c_cpFile, SoundData* In_pData);

	DWORD ReadMP3Format(HANDLE In_hFile, MP3FormatInfo* In_pFormat);

	DWORD ReadMP3FrameHeader(HANDLE In_hFile, DWORD In_dwSeek, MP3FrameInfo* In_pFrame) ;

	DWORD ReadMP3Data(HANDLE In_hFile, DWORD In_dwSeek, DWORD In_dwSize, MP3FrameInfo* In_pFrame, SoundData* In_pData);

	using SoundKey = std::pair<std::string, SoundData>;	// サウンドキー
	using SoundMap = std::map<std::string, SoundData>;	// サウンドマップ
	const BYTE CMP_MATCH;	// 定数

	ComPtr<IXAudio2> m_pXAudio{};
	IXAudio2MasteringVoice* m_pMasterVoice{};
	SoundMap m_soundMap{};
	float m_fBGMVolume{};
	float m_fSEVolume{};
};
