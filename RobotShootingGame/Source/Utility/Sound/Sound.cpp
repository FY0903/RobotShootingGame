/*+===================================================================
	File: Sound.cpp
	Summary: Soundクラスの実装ファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/08 11:56 初回作成
            26/01/16 11:26 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Sound.hpp"

HRESULT Sound::Init()
{
	HRESULT hr = E_FAIL;

	// COMオブジェクト初期化
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr)) return hr;

	// XAUDIO2初期化
	hr = XAudio2Create(&m_pXAudio);
	if (FAILED(hr)) return hr;

	// マスターボイス(ソース)作成
	hr = m_pXAudio->CreateMasteringVoice(&m_pMasterVoice);

	return hr;
}

void Sound::Uninit()
{
	// サウンドデータの削除
	SoundMap::iterator itr = m_soundMap.begin();
	for (; itr != m_soundMap.end(); ++itr)
	{
		// ソースボイスの停止
		itr->second.pSourceVoice->Stop();
		// ストップした後、バッファが残るので削除
		itr->second.pSourceVoice->FlushSourceBuffers();

		// サウンドデータの削除
		itr->second.data.pBuffer = nullptr;
	}

	// XAudio2のオブジェクトを削除
	if (m_pMasterVoice != NULL)
	{
		m_pMasterVoice->DestroyVoice();
		m_pMasterVoice = NULL;
	}

	m_pXAudio = nullptr;
	m_soundMap.clear();
}

XAUDIO2_BUFFER* Sound::Load(const std::string& soundName, const std::string& fileName, const bool& In_bIsSE, const bool& In_bLoop)
{
	SoundData data;

	// 読み込み済みのサウンドファイルか確認
	auto itr = m_soundMap.find(soundName);
	if (itr != m_soundMap.end())
	{
		// 読み込み済みのサウンドファイルを返す
		return &itr->second.data.sound;
	}

	// 拡張子ごとに読み込み処理実行
	HRESULT hr = E_FAIL;
	LPSTR ext = PathFindExtension(fileName.c_str());
	if (ext != NULL)
	{
		if (memcmp(ext, ".wav", 4) == 0)
			hr = LoadWav(fileName.c_str(), &data);
		else if (memcmp(ext, ".mp3", 4) == 0)
			hr = LoadMP3(fileName.c_str(), &data);
	}

	if (FAILED(hr))	return NULL;

	//--- ファイルパスを登録
	data.data.filePath = fileName.c_str();
	//--- バッファー作成
	ZeroMemory(&data.data.sound, sizeof(data.data.sound));
	// サウンドデータのバイト数
	data.data.sound.AudioBytes = data.data.bufSize;
	// サウンドデータの先頭アドレス
	data.data.sound.pAudioData = data.data.pBuffer.get();
	// ループ指定
	if (In_bLoop)
	{
		data.data.sound.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	data.data.sound.Flags = XAUDIO2_END_OF_STREAM;

	m_pXAudio->CreateSourceVoice(&data.pSourceVoice, &data.data.format);

	In_bIsSE ? data.volume = m_fSEVolume : data.volume = m_fBGMVolume;

	data.pSourceVoice->SetVolume(data.volume);

	data.IsSE = In_bIsSE;

	// 読み込み済みデータとして登録
	m_soundMap.insert(SoundKey(soundName, data));

	return &m_soundMap.at(soundName).data.sound;
}

void Sound::Play(const std::string& soundName)
{
	IXAudio2SourceVoice* pSource;

	// 再生するデータを探索
	auto itr = m_soundMap.find(soundName);
	// 該当データなし
	if (itr == m_soundMap.end()) return;

	// フォーマットを指定してソースを作成
	/*----------
	* WAVEFORMATEX
	* wFormatTag
	*  フォーマット
	* nChannels
	*  チャンネル数
	*  1... モノラル
	*  2... ステレオ
	* wBitsPerSample
	*  1サンプルあたりのビット数
	*  8...128を無音とする0~255
	*  16...0を無音とする-32768~32767
	* nSamplesPerSec
	*  サンプリングレート
	*  (1秒あたりのサンプル数
	*  単位はHz(ヘルツ
	*----------*/
	if (FAILED(m_pXAudio->CreateSourceVoice(&pSource, &itr->second.data.format)))
		return;
	pSource->SubmitSourceBuffer(&itr->second.data.sound);

	// 再生
	pSource->Start();

	itr->second.pSourceVoice = pSource;
	itr->second.IsSE ?
		itr->second.pSourceVoice->SetVolume(itr->second.volume * m_fSEVolume) :
		itr->second.pSourceVoice->SetVolume(itr->second.volume * m_fSEVolume);
}

bool Sound::IsPlaying(const std::string& soundName) const
{
	XAUDIO2_VOICE_STATE state;
	// 確認するデータを探索
	auto itr = m_soundMap.find(soundName);
	if (itr == m_soundMap.end())
	{
		// 該当のデータなし
		return false;
	}

	itr->second.pSourceVoice->GetState(&state, 0); // ステータスの取得

	// 再生中かどうか
	if (state.BuffersQueued)
		return true;
	else
		return false;
}

void Sound::Stop(const std::string& soundName, bool isFlag)
{
	if (isFlag)
	{
		// 特定のキー以外のサウンドデータを削除
		for (auto itr = m_soundMap.begin(); itr != m_soundMap.end();)
		{
			if (itr->first != soundName)
			{
				itr->second.pSourceVoice->Stop();
				// ストップした後、バッファが残るので削除
				itr->second.pSourceVoice->FlushSourceBuffers();

				++itr;
			}
			else
			{
				++itr;
			}
		}
	}
	else
	{
		// 停止するデータを探索
		auto itr = m_soundMap.find(soundName);
		if (itr == m_soundMap.end() || itr->second.pSourceVoice == nullptr)
			return;

		itr->second.pSourceVoice->Stop();
		// ストップした後、バッファが残るので削除
		itr->second.pSourceVoice->FlushSourceBuffers();
	}
}

void Sound::SetVolume(const std::string& soundName, float volume)
{
	// ボリュームを設定するデータを探索
	auto itr = m_soundMap.find(soundName);
	if (itr == m_soundMap.end() || itr->second.pSourceVoice == nullptr)
		return;

	itr->second.volume = volume;
	itr->second.pSourceVoice->SetVolume(volume);
}

void Sound::UnLoad(const std::string& soundName)
{
	// サウンドデータの削除
	auto itr = m_soundMap.find(soundName);
	if (itr != m_soundMap.end())
	{
		itr->second.data.pBuffer = nullptr;
		m_soundMap.erase(itr);
	}
}

void Sound::SetSEVolume(const float& volume)
{
	m_fSEVolume = roundf(volume * 10) / 10;
	// ------------------------------
	//	0.0f ~ 1.0fに収まるように
	// ------------------------------
	if (m_fSEVolume < 0.0f) m_fSEVolume = 0.0f;
	if (m_fSEVolume > 1.0f) m_fSEVolume = 1.0f;

	// ------------------------------
	//	音声のシーク開始
	// ------------------------------
	for (auto itr = m_soundMap.begin(); itr != m_soundMap.end(); ++itr)
	{
		if (itr->second.pSourceVoice)
		{
			if (itr->second.IsSE)	// サウンド属性がSEだったら
				itr->second.pSourceVoice->SetVolume(itr->second.volume * m_fSEVolume);
		}
	}
}

void Sound::SetBGMVolume(const float& volume)
{
	m_fBGMVolume = roundf(volume * 10) / 10;

	// ------------------------------
	//	0.0f ~ 1.0fに収まるように
	// ------------------------------
	if (volume < 0.0f) m_fBGMVolume = 0.0f;
	if (volume > 1.0f) m_fBGMVolume = 1.0f;

	// ------------------------------
	//	音声のシーク開始
	// ------------------------------
	for (auto itr = m_soundMap.begin(); itr != m_soundMap.end(); ++itr)
	{
		if (itr->second.pSourceVoice)
		{
			if (!itr->second.IsSE)	// サウンド属性がSEじゃなかったら
				itr->second.pSourceVoice->SetVolume(itr->second.volume * m_fBGMVolume);
		}
	}
}

Sound::Sound()
	: CMP_MATCH(0), m_fBGMVolume(0.5f), m_fSEVolume(0.5f)
{
}

Sound::~Sound()
{
	Uninit();
}

HRESULT Sound::LoadWav(const char* filePath, SoundData* data)
{
	HMMIO hMmio = NULL;
	MMIOINFO mmioInfo;
	MMRESULT mmRes;

	// WAVEファイルオープン
	memset(&mmioInfo, 0, sizeof(MMIOINFO));
	hMmio = mmioOpen(const_cast<char*>(filePath), &mmioInfo, MMIO_READ);
	if (hMmio == NULL) return E_FAIL;

	// RIFFチャンク検索
	MMCKINFO riffChunk;
	riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmRes = mmioDescend(hMmio, &riffChunk, NULL, MMIO_FINDRIFF);
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return E_FAIL;
	}

	// フォーマットチャンク検索
	MMCKINFO formatChunk;
	formatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmRes = mmioDescend(hMmio, &formatChunk, &riffChunk, MMIO_FINDCHUNK);
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return E_FAIL;
	}

	// フォーマット取得
	DWORD formatSize = formatChunk.cksize;
	DWORD size = mmioRead(hMmio, reinterpret_cast<HPSTR>(&data->data.format), formatSize);
	if (size != formatSize)
	{
		mmioClose(hMmio, 0);
		return E_FAIL;
	}

	// RIFFチャンクに移動
	mmioAscend(hMmio, &formatChunk, 0);


	// データチャンク検索
	MMCKINFO dataChunk;
	dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmRes = mmioDescend(hMmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK);
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return E_FAIL;
	}

	// データ取得
	data->data.bufSize = dataChunk.cksize;
	data->data.pBuffer.reset(new BYTE[data->data.bufSize]);
	size = mmioRead(hMmio, reinterpret_cast<HPSTR>(data->data.pBuffer.get()), data->data.bufSize);
	if (size != dataChunk.cksize)
	{
		data->data.bufSize = 0;
		if (data->data.pBuffer != NULL)
		{
			//delete[] pData->data.pBuffer;
			data->data.pBuffer = nullptr;
			data->data.pBuffer;
		}
		return E_FAIL;
	}

	mmioClose(hMmio, 0);
	return S_OK;
}

HRESULT Sound::LoadMP3(const char* filePath, SoundData* data)
{
	HANDLE hFile; // ファイルポインタ
	DWORD readSize; // 読み込みサイズ

	// 読み込み
	hFile = CreateFile(
		filePath, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
	);
	if (hFile == INVALID_HANDLE_VALUE) return E_FAIL;

	// ファイルフォーマット読み込み
	MP3FormatInfo format;
	readSize = ReadMP3Format(hFile, &format);
	if (readSize == 0) return E_FAIL;

	// サウンドフレームヘッダ読み込み
	MP3FrameInfo frame;
	readSize = ReadMP3FrameHeader(hFile, format.offset, &frame);
	if (readSize == 0) return E_FAIL;

	// サウンドデータ読み込み
	readSize = ReadMP3Data(hFile, format.offset, format.dataSize, &frame, data);
	if (readSize == 0) return E_FAIL;

	return S_OK;
}

DWORD Sound::ReadMP3Format(HANDLE file, MP3FormatInfo* format)
{
	DWORD readSize;

	// MP3ファイルサイズ
	DWORD fileSize = GetFileSize(file, NULL);

	// ①ヘッダー/フッターの有無を調べる
	/*----------
	* ID3v1
	*  -末尾にタグがあるorタグなし
	*  [MP3データ]
	*		or
	*	[MP3データ]["TAG"(3byte)][タグ情報(125byte)]
	* ID3v2
	*  -先頭にタグがある
	*	["ID3"(3byte)][バージョン(2byte)][フラグ(1byte)][タグサイズ(4byte)][拡張ヘッダ][MP3データ]
	*----------*/
	const BYTE ID3V2_HEADER_SIZE = 10;
	BYTE header[ID3V2_HEADER_SIZE];	// ヘッダー情報
	static_cast<void>(ReadFile(file, header, sizeof(header), &readSize, NULL));

	// タグをチェックし、MP3データの位置、サイズを計算
	const char* ID3V1_TAG = "TAG";
	const char* ID3V2_TAG = "ID3";
	const BYTE MP3_TAG_SIZE = 3;
	if (memcmp(header, ID3V2_TAG, MP3_TAG_SIZE) == CMP_MATCH)
	{
		// ID3v2ヘッダー情報解析
		/*----------
		* [拡張ヘッダ]のデータサイズについて
		*  [タグサイズ]のデータ構造
		*	 [0AAAAAAA][0BBBBBBB][0CCCCCCC][0DDDDDDD]
		*	実際のデータサイズ
		*	 0x0000AAAAAAABBBBBBBCCCCCCCDDDDDDD
		*
		* - データの最上位ビットは必ず0
		*	 実際のサイズは、0を省いて詰めたもの
		*----------*/
		DWORD exHeaderSize =
			(header[6] << 21) | (header[7] << 14) |
			(header[8] << 7) | (header[9]);
		format->offset = exHeaderSize + ID3V2_HEADER_SIZE;
		format->dataSize = fileSize - format->offset;
	}
	else
	{
		// ID3v1フッター情報解析
		const BYTE ID3V1_FOOTER_SIZE = 128;
		BYTE tag[MP3_TAG_SIZE];
		SetFilePointer(file, fileSize - ID3V1_FOOTER_SIZE, NULL, FILE_BEGIN);
		static_cast<void>(ReadFile(file, tag, MP3_TAG_SIZE, &readSize, NULL));
		format->offset = 0;
		if (memcmp(tag, ID3V1_TAG, MP3_TAG_SIZE) == CMP_MATCH)
		{
			format->dataSize = fileSize - 128;
		}
		else
		{
			format->dataSize = fileSize;
		}
	}
	return format->dataSize;
}

DWORD Sound::ReadMP3FrameHeader(HANDLE file, DWORD seek, MP3FrameInfo* frame)
{
	DWORD readSize;

	// ①フレームヘッダから情報を取得
	/*----------
	 * MP3データの構造
	 *	[フレームヘッダ(4byte)][データ]
	 *	[フレームヘッダ(4byte)][データ]
	 *	[フレームヘッダ(4byte)][データ]
	 *	...(繰り返し
	 *----------*/
	 // MP3データ位置へ移動
	SetFilePointer(file, seek, NULL, FILE_BEGIN);

	/*----------
	 * フレームヘッダ情報
	 * http://www.cactussoft.co.jp/Sarbo/divMPeg3UnmanageHeader.html
	 *	[AAAAAAAA][AAABBCCD][EEEEFFGH][IIJJKLMM]
	 *	 A - 同期ビット(必ず１)
	 *	 B - MP3のバージョン
	 *		00 - MPEG2.5
	 *		01 - 予約
	 *		10 - MPEG2
	 *		11 - MPEG1
	 *	 C - レイヤー
	 *		00 - 予約
	 *		01 - Layer3
	 *		10 - Layer2
	 *		11 - Layer1
	 *	 D - CRC誤り検出の有無
	 *	 E - ビットレート
	 *	 F - サンプリング周波数
	 *	 G - パディング(フレームの最後のデータが0xFFだった場合、
	 *		 次のフレームの頭とつながるため、フレームの最後にNULLが挿入される
	 *		 実際にデータが挿入されたかどうかの情報をもつ
	 *	 H - 拡張(未使用
	 *	 I - チャンネルモード
	 *	 J - 拡張
	 *	 K - 著作権の有無
	 *	 L - オリジナル(コピーorオリジナル
	 *	 M - 強調
	 *
	 *	- 最低限必要な情報はB,C,E,F,G,I
	 *----------*/
	const BYTE FRAME_HEADER_SIZE = 4;
	BYTE frameHeader[FRAME_HEADER_SIZE];
	static_cast<void>(ReadFile(file, frameHeader, FRAME_HEADER_SIZE, &readSize, NULL));

	// 同期ビットチェック
	if (!(frameHeader[0] == 0xFF && (frameHeader[1] & 0xE0) == 0xE0)) return 0;

	// バージョン
	BYTE version = (frameHeader[1] >> 3) & 0b11;

	// レイヤー
	BYTE layer = (frameHeader[1] >> 1) & 0b11;

	// ビットレート(単位はkbit/sec
	const int bitRateTable[][16] = {
		// MPEG1, Layer1
		{ 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, -1 },
		// MPEG1, Layer2
		{ 0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, -1 },
		// MPEG1, Layer3
		{ 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, -1 },
		// MPEG2, Layer1
		{ 0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, -1 },
		// MPEG2/2.5, Layer2,3
		{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, -1 }
	};
	BYTE bitRateTableIndex;
	if (version == 0b11)
		bitRateTableIndex = 0b11 - layer;
	else if (version == 0b10 && layer == 0b11)
		bitRateTableIndex = 3;
	else
		bitRateTableIndex = 4;

	WORD bitRate = bitRateTable[bitRateTableIndex][frameHeader[2] >> 4];

	// サンプリングレート
	const int sampleRateTable[][4] = {
		// MPEG1
		{ 44100, 48000, 32000, -1 },
		// MPEG2
		{ 22050, 24000, 16000, -1 },
		// MPEG2.5
		{ 11025, 12000, 8000, -1 },
	};
	BYTE sampleRateTableIndex;
	switch (version)
	{
	case 0b00: sampleRateTableIndex = 2; break;
	case 0b10: sampleRateTableIndex = 1; break;
	case 0b11: sampleRateTableIndex = 0; break;
	default: return E_FAIL;
	}
	WORD sampleRate = sampleRateTable[sampleRateTableIndex][(frameHeader[2] >> 2) & 0b11];

	// パディング
	BYTE padding = (frameHeader[2] >> 1) & 0b01;

	// チャンネル
	BYTE channel = frameHeader[3] >> 6;


	// フレームサイズ
	// ビットレートはkbit/secなのでbit/secに変換するために1000倍する
	const int samplePerFrame[][4] = {
		// layer1
		{ 384, 384, 384, -1 },
		// layer2
		{ 1152, 1152, 1152, -1 },
		// layer3
		{ 1152, 576, 576, -1 }
	};
	WORD frameBlockSize = ((samplePerFrame[0b11 - layer][sampleRateTableIndex] * bitRate * 1000 / 8) / sampleRate) + padding;

	// ②取得した情報をもとにMEPGLAYER3WAVEFORMATにデータ設定
	/*----------
	 *	MPEGLAYER3WAVEFORMAT
	 *	http://www.cactussoft.co.jp/Sarbo/divMPeg3UnmanageStruct.html
	 *	 - mpegオーディオレイヤー3(mp3)を扱うために拡張されたWAVEFORMAT構造体
	 *	.wfx.cbSize				- 構造体の拡張サイズ、MPEGLAYER3_WFX_EXTRA_BYTESを指定
	 *	.wfx.nChannels			- チャンネル数、モノラル＝１、ステレオ＝２
	 *	.wfx.wFormatTag			- mp3を表す、WAVE_FORMAT_MPEGLAYER3を指定
	 *	.wfx.nBlockAlign		- 1を指定
	 *	.wfx.wBitsPerSample		- 0を指定
	 *	.wfx.nSamplesPerSec		- サンプリング周波数
	 *	.wfx.nAvgBytesPerSec	- 1秒間に必要なデータサイズ
	 *	wID						- MPEGLAYER3_ID_MPEGを指定
	 *	fdwFlags				- パディングの挿入があれば指定
	 *	nFramesPerBlock			- 1つのブロックに配置するフレーム数
	 *	nBlockSize				- ブロックサイズ(フレームサイズ * フレーム数)
	 *	nCodecDelay				- 1393(0x571)を指定
	 *----------*/

	 // channel
	 // sample
	 // bitRate
	 // padding
	frame->channel = channel == 0x11 ? 1 : 2;
	frame->sampleRate = sampleRate;
	frame->bitRate = bitRate;
	frame->padding = padding;
	frame->frameSize = frameBlockSize;

	return frame->frameSize;
}

DWORD Sound::ReadMP3Data(HANDLE file, DWORD seek, DWORD size, MP3FrameInfo* frame, SoundData* data)
{
	// 変換フォーマット作成
	MPEGLAYER3WAVEFORMAT mp3WavFormat;
	mp3WavFormat.wfx.cbSize = MPEGLAYER3_WFX_EXTRA_BYTES;
	mp3WavFormat.wfx.nChannels = frame->channel;
	mp3WavFormat.wfx.wFormatTag = WAVE_FORMAT_MPEGLAYER3;
	mp3WavFormat.wfx.nBlockAlign = 1;
	mp3WavFormat.wfx.wBitsPerSample = 0;
	mp3WavFormat.wfx.nSamplesPerSec = frame->sampleRate;
	mp3WavFormat.wfx.nAvgBytesPerSec = (frame->bitRate * 1000) / 8;

	mp3WavFormat.wID = MPEGLAYER3_ID_MPEG;
	mp3WavFormat.fdwFlags = frame->padding ? MPEGLAYER3_FLAG_PADDING_ON : MPEGLAYER3_FLAG_PADDING_OFF;
	mp3WavFormat.nFramesPerBlock = 1;
	mp3WavFormat.nBlockSize = static_cast<WORD>(frame->frameSize * mp3WavFormat.nFramesPerBlock);
	mp3WavFormat.nCodecDelay = 0x571;

	// mp3をwavへ変換可能か
	// 変換可能であれば,wavFormatへデータを設定
	WAVEFORMATEX wavFormat;
	wavFormat.wFormatTag = WAVE_FORMAT_PCM;
	MMRESULT mmr;
	mmr = acmFormatSuggest(NULL, &mp3WavFormat.wfx, &wavFormat, sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_WFORMATTAG);
	if (mmr != MMSYSERR_NOERROR) return 0;

	// ACMストリームオープン
	// mp3からwavへフォーマット変換
	HACMSTREAM has;
	mmr = acmStreamOpen(&has, NULL, &mp3WavFormat.wfx, &wavFormat, NULL, NULL, NULL, 0);

	// MP3のブロックサイズからWAVE形式へデコード後のサイズを取得
	DWORD waveBlockSize;
	acmStreamSize(has, size, &waveBlockSize, ACM_STREAMSIZEF_SOURCE);

	// 変換データセット
	ACMSTREAMHEADER ash = { 0 };
	ash.cbStruct = sizeof(ACMSTREAMHEADER);
	ash.cbSrcLength = size;
	ash.pbSrc = new BYTE[ash.cbSrcLength];
	ash.cbDstLength = waveBlockSize;
	ash.pbDst = new BYTE[ash.cbDstLength];

	// デコード
	acmStreamPrepareHeader(has, &ash, 0);
	DWORD readSize;
	SetFilePointer(file, seek, NULL, FILE_BEGIN);
	static_cast<void>(ReadFile(file, ash.pbSrc, ash.cbSrcLength, &readSize, NULL));
	mmr = acmStreamConvert(has, &ash, 0);
	acmStreamUnprepareHeader(has, &ash, 0);
	acmStreamClose(has, 0);

	// wavデータコピー
	if (ash.cbDstLengthUsed > 0)
	{
		data->data.bufSize = ash.cbDstLengthUsed;
		data->data.pBuffer.reset(new BYTE[data->data.bufSize]);
		data->data.format = wavFormat;
		memcpy_s(data->data.pBuffer.get(), data->data.bufSize,
			ash.pbDst, ash.cbDstLengthUsed);
	}

	delete[] ash.pbSrc;
	delete[] ash.pbDst;

	CloseHandle(file);

	return ash.cbSrcLengthUsed;
}
