#pragma once
#include <vector>
#include "GameObjectBase.h"
#include "BackgroundRect.h"

namespace GameObject
{
    class RectMgr : public Base
    {
    public:
        RectMgr();
        ~RectMgr();
        // Public Method
        void Init(){}
        void Update()override;
        void Draw()const override;

        void Run() { remainingRunNum = RunNum; }

        // Private Variable
        std::vector<BackgroundRect*> pool;  // プール
        const uint8_t pool_size;            // プールのサイズ
        const float DistDepth;              // 奥行きの設置間隔
        const float MaxScaleWidth;          // 横の最大の拡大率
        const float MaxScaleHeight;         // 縦の最大の拡大率
        const float Duration;               // 設置間隔を移動するのにかかる時間
        const uint8_t RunNum;               // 実行回数

    private:
        float timeCounter;                  // タイムカウンター
        short remainingRunNum;              // 残り実行回数

        /// <summary>
        /// 四角を初期設定(位置,拡大率)にする
        /// </summary>
        void InitRects();
    };
}