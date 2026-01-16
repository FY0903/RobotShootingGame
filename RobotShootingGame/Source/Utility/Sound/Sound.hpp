/*+===================================================================
	File: Sound.hpp
	Summary: Soundクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/08 11:56 初回作成
            26/01/16 11:26 コメント記載
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
	/**
	 * @brief 初期化処理
	 * @return HRESULT 初期化の成否を表す結果コード。
	 */
	HRESULT Init();

	/**
	 * @brief 終了処理
	 */
	void Uninit();

	/**
	 * @brief 指定した音声ファイルを読み込み、再生に使用するXAUDIO2_BUFFERへのポインタを返します。
	 * @param soundName サウンドを識別するための名前（論理名）。サウンド管理や検索に使用されます。
	 * @param fileName 読み込む音声ファイルのパスまたはファイル名。
	 * @param isSE 効果音（SE）として扱う場合はtrue。
	 * @param isLoop ループ再生する場合はtrue。
	 * @return 読み込みに成功した場合はXAUDIO2_BUFFERへのポインタを返します。読み込みに失敗した場合はnullptrを返す可能性があります。
	 */
	XAUDIO2_BUFFER* Load(const std::string& soundName, const std::string& fileName,
		const bool& isSE = true, const bool& isLoop = false);

	/**
	 * @brief 指定した名前の音声を再生します。
	 * @param soundName 再生する音声の名前を表す文字列参照。
	 */
	void Play(const std::string& soundName);

	/**
	 * @brief 指定したサウンド名のサウンドが現在再生中かどうかを判定します（メソッドは const です）。
	 * @param soundName 確認するサウンドの名前。関数内で変更されません。
	 * @return サウンドが再生中であれば true、そうでなければ false を返します。
	 */
	bool IsPlaying(const std::string& soundName) const;

	/**
	 * @brief 指定した名前の音声を停止します。
	 * @param soundName 停止する音声の名前を表す文字列参照。
	 * @param isFlag 停止フラグ。
	 */
	void Stop(const std::string& soundName, bool isFlag = false);

	/**
	 * @brief 指定したサウンドの音量を設定します。
	 * @param soundName 音量を設定するサウンドの名前を表す文字列参照。
	 * @param volume 設定する音量の値（0.0f から 1.0f の範囲）。
	 */
	void SetVolume(const std::string& soundName, float volume);

	/**
	 * @brief 指定した名前の音声データをアンロード（解放）します。
	 * @param soundName アンロードする音声データの名前を表す文字列参照。
	 */
	void UnLoad(const std::string& soundName);

	/**
	 * @brief XAudio2のインターフェースを取得します（メソッドは const です）。
	 * @return IXAudio2型のポインタを返します。
	 */
	inline IXAudio2* GetXAudio() const { return m_pXAudio.Get(); }

	/**
	 * @brief BGMの音量を取得します（メソッドは const です）。
	 * @return BGMの音量を表すfloat型の定数参照を返します。
	 */
	inline const float& GetBGMVolume() const { return m_fBGMVolume; }

	/**
	 * @brief SEの音量を取得します（メソッドは const です）。
	 * @return SEの音量を表すfloat型の定数参照を返します。
	 */
	inline const float& GetSEVolume() const { return m_fSEVolume; }

	/**
	 * @brief SEの音量を設定します。
	 * @param volume 設定するSEの音量を表すfloat型の定数参照。
	 */
	void SetSEVolume(const float& volume);

	/**
	 * @brief BGMの音量を設定します。
	 * @param volume 設定するBGMの音量を表すfloat型の定数参照。
	 */
	void SetBGMVolume(const float& volume);

private:
	/**
	 * @brief サウンドデータを格納するための構造体です。
	 */
	struct Data
	{
		std::string filePath;				//ファイルパス
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
		Data data{};							// サウンドデータ
		IXAudio2SourceVoice* pSourceVoice{};	// ソースボイス
		float volume{};							// 音量
		bool IsSE{};							// SEフラグ
	};

	/**
	 * @brief MP3データのオフセットとサイズ情報を保持する構造体です。
	 */
	struct MP3FormatInfo
	{
		DWORD offset{};		// データオフセット
		DWORD dataSize{};	// データサイズ
	};

	/**
	 * @brief MP3フレーム情報を保持する構造体です。
	 */
	struct MP3FrameInfo
	{
		BYTE channel{};			// チャンネル数
		BYTE padding{};			// パディング
		DWORD sampleRate{};		// サンプリングレート
		DWORD bitRate{};		// ビットレート
		DWORD frameSize{};		// フレームサイズ
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

	/**
	 * @brief WAVファイルを読み込み、SoundData構造体に格納します。
	 * @param filePath 読み込むWAVファイルのパス。
	 * @param data SoundData構造体へのポインタ。
	 * @return HRESULT 読み込みの成否を表す結果コード。
	 */
	HRESULT LoadWav(const char* filePath, SoundData* data);

	/**
	 * @brief MP3ファイルを読み込み、SoundData構造体に格納します。
	 * @param filePath 読み込むMP3ファイルのパス。
	 * @param data SoundData構造体へのポインタ。
	 * @return HRESULT 読み込みの成否を表す結果コード。
	 */
	HRESULT LoadMP3(const char* filePath, SoundData* data);

	/**
	 * @brief MP3ファイルのフォーマット情報を読み取ります。
	 * @param file ファイルハンドル。
	 * @param format MP3FormatInfo構造体へのポインタ。
	 * @return DWORD 読み取ったデータのサイズ。
	 */
	DWORD ReadMP3Format(HANDLE file, MP3FormatInfo* format);

	/**
	 * @brief MP3フレームヘッダーを読み取ります。
	 * @param file ファイルハンドル。
	 * @param seek 読み取り開始位置。
	 * @param frame MP3FrameInfo構造体へのポインタ。
	 * @return DWORD 読み取ったデータのサイズ。
	 */
	DWORD ReadMP3FrameHeader(HANDLE file, DWORD seek, MP3FrameInfo* frame);

	/**
	 * @brief MP3データを読み取ります。
	 * @param file ファイルハンドル。
	 * @param seek 読み取り開始位置。
	 * @param size 読み取るデータのサイズ。
	 * @param frame MP3FrameInfo構造体へのポインタ。
	 * @param data SoundData構造体へのポインタ。
	 * @return DWORD 読み取ったデータのサイズ。
	 */
	DWORD ReadMP3Data(HANDLE file, DWORD seek, DWORD size, MP3FrameInfo* frame, SoundData* data);

	using SoundKey = std::pair<std::string, SoundData>;				// サウンドキー
	using SoundMap = std::unordered_map<std::string, SoundData>;	// サウンドマップ

	const BYTE CMP_MATCH;	// 定数

	ComPtr<IXAudio2> m_pXAudio{};				// XAudio2インターフェース
	IXAudio2MasteringVoice* m_pMasterVoice{};	// マスターボイス
	SoundMap m_soundMap{};						// サウンドマップ
	float m_fBGMVolume{};						// BGM音量
	float m_fSEVolume{};						// SE音量
};
