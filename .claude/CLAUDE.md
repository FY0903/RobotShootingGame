# プロジェクト概要

DirectX12 ロボットシューティングゲーム（C++ / Visual Studio）

## コーディング規約

- 改行コードは **CRLF** を使用する
- 既存のコードスタイル・命名規則に合わせる
- Singletonパターンを基盤クラスに使用（`Singleton<T>`）
- コメントは日本語で記述

## ビルド環境

- Visual Studio（Windows）
- DirectX 12 / D3D12
- Feature Level 11_0

## ブランチ運用

- メインブランチ: `develop`
- 機能ブランチ: `feature/*`

## プロジェクト構成

```
Source/
├── System/         # エンジン基盤（Engine, Render, Window, ThreadPool, RenderPass）
├── Game/           # ゲームロジック（Actor, Scene）
└── Utility/        # ユーティリティ（Component, Manager, Buffer, etc.）
```

## 描画パイプライン

ShadowMap → Opaque（GBuffer） → Transparent → PostProcess → BackBuffer
\r