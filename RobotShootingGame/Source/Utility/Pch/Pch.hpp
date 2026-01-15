/*+===================================================================
	File: Pch.hpp
	Summary: プリコンパイルヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/02 12:56 初回作成
			26/01/15 19:09 コメント記載
===================================================================+*/
#pragma once

#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include <cstdint>
#include <cassert>

// メモリリークの検出
#if defined(_DEBUG) || defined(DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// COM
#include <wrl/client.h>
#include <initguid.h>

// timer
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// ------------------------------
//	STL
// ------------------------------
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <algorithm>
#include <array>
#include <functional>
#include <map>
#include <any>
#include <set>
#include <list>
#include <mutex>
#include <cmath>
#include <future>
#include <thread>
#include <filesystem>
#include <unordered_map>
#include <chrono>
#include <iterator>
#include <typeindex>
#include <random>

// ------------------------------
//	xaudio2
// ------------------------------
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

// ------------------------------
//    DirectX
// ------------------------------
#include <DirectXMath.h>
#include <SimpleMath.h>

// ------------------------------
//	original
// ------------------------------
#include "Utility/Singleton/Singleton.hpp"
#include "System/main.hpp"
#include "Utility/ComPtr.h"
#include "Utility/Time/Time.hpp"
