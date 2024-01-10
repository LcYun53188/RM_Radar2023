#include "../include/MovementDetector.h"
// TODO: 此部分源码需要并行优化

MovementDetector::MovementDetector()
{
    vector<vector<vector<float>>> backgroundHistory(MDHistorySize, vector<vector<float>>(int(ImageH / _blockSizeH), vector<float>(int(ImageW / _blockSizeW), 0)));
    this->backgroundHistory.swap(backgroundHistory);
    this->tempDepth = Mat(int(ImageH / _blockSizeH), int(ImageW / _blockSizeW), CV_8U);
}

MovementDetector::~MovementDetector()
{
}

vector<Rect> MovementDetector::applyMovementDetector(vector<vector<float>> &input)
{
    if (this->historyCount < MDHistorySize)
    {
        for (int i = 0; i + _blockSizeH < ImageH; i += _blockSizeH)
        {
            for (int j = 0; j + _blockSizeW < ImageW; j += _blockSizeW)
            {
                float tempbox[_blockSizeH * _blockSizeW];
                int count = 0;
                for (int m = i; m < i + _blockSizeH; ++m)
                {
                    for (int n = j; n < j + _blockSizeW; ++n)
                    {
                        if (input[m][n] == 0)
                            continue;
                        tempbox[count] = input[m][n];
                        ++count;
                    }
                }
                if (count == 0)
                    continue;
                float tempNum = 0;
                for (int k = 0; k < count; ++k)
                    tempNum += tempbox[k];
                float currentNum = tempNum / count;
                if (isnan(currentNum))
                    continue;
                this->backgroundHistory[historyCount][i / _blockSizeH][j / _blockSizeW] = currentNum;
            }
        }
        ++this->historyCount;
        return {};
    }
    else if (this->historyCount == MDHistorySize)
    {
        this->buildBackground();
        ++this->historyCount;
        return {};
    }
    else
    {
        float tmpImg[int(ImageH / _blockSizeH)][int(ImageW / _blockSizeW)];
        for (int i = 0; i + _blockSizeH < ImageH; i += _blockSizeH)
        {
            for (int j = 0; j + _blockSizeW < ImageW; j += _blockSizeW)
            {
                float tempbox[_blockSizeH * _blockSizeW];
                int count = 0;
                for (int m = i; m < i + _blockSizeH; ++m)
                {
                    for (int n = j; n < j + _blockSizeW; ++n)
                    {
                        if (input[m][n] == 0)
                            continue;
                        tempbox[count] = input[m][n];
                        count++;
                    }
                }
                if (count == 0)
                    continue;
                float tempNum = 0;
                for (int k = 0; k < count; ++k)
                    tempNum += tempbox[k];
                float currentNum = tempNum / count;
                if (isnan(currentNum))
                    continue;
                tmpImg[i / _blockSizeH][j / _blockSizeW] = currentNum;
            }
        }
        return this->detectMovementTarget(tmpImg);
    }
}

void MovementDetector::buildBackground()
{
    for (int i = 0; i < MDHistorySize; ++i)
    {
        for (int m = 0; m < int(ImageH / _blockSizeH); ++m)
        {
            for (int n = 0; n < int(ImageW / _blockSizeW); ++n)
            {
                this->depthBackground[m][n] += this->backgroundHistory[i][m][n] / MDHistorySize;
            }
        }
    }
    for (int i = 0; i < MDHistorySize; ++i)
    {
        for (int m = 0; m < int(ImageH / _blockSizeH); ++m)
        {
            for (int n = 0; n < int(ImageW / _blockSizeW); ++n)
            {
                this->offsetMap[m][n] += abs(this->backgroundHistory[i][m][n] - this->depthBackground[m][n]) / MDHistorySize;
            }
        }
    }
}

vector<Rect> MovementDetector::detectMovementTarget(float input[int(ImageH / _blockSizeH)][int(ImageW / _blockSizeW)])
{
    for (int m = 0; m < int(ImageH / _blockSizeH); ++m)
    {
        for (int n = 0; n < int(ImageW / _blockSizeW); ++n)
        {
            this->movementMask[m][n] = (fabs(this->depthBackground[m][n]) > Epsilon && fabs(input[m][n]) > Epsilon && (abs(input[m][n] - this->depthBackground[m][n]) > this->offsetMap[m][n] * OffsetRatio)) ? 1 : 0;
        }
    }
    for (int m = 0; m < int(ImageH / _blockSizeH); ++m)
    {
        uchar *pdata = this->tempDepth.data + m * this->tempDepth.step;
        for (int n = 0; n < int(ImageW / _blockSizeW); ++n)
        {
            pdata[0] = this->movementMask[m][n];
            pdata += 1;
        }
    }
    morphologyEx(this->tempDepth, this->tempDepth, MORPH_CLOSE, this->kernel);
    morphologyEx(this->tempDepth, this->tempDepth, MORPH_OPEN, this->kernel);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(this->tempDepth, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    vector<Rect>().swap(this->movementTargets);
    for (const auto &it : contours)
    {
        this->movementTargets.emplace_back(this->rebuildRect(boundingRect(it)));
    }

    // TODO:结果有效性评估
    return this->movementTargets;
}
// TODO:待验证
Rect MovementDetector::rebuildRect(Rect input)
{
    // TODO:进阶预测框滤除
    input = reMapRect(input, _blockSizeW, _blockSizeH);
    input = rectCenterScale(input, Size(int(input.width * MTBoxRatio), int(input.height * MTBoxRatio)));
    // TODO:重合框合并
    // TODO:历史帧预测框判断（IOU）
    return input;
}

bool MovementDetector::_ifHistoryBuild()
{
    return this->historyCount == MDHistorySize + 1;
} 