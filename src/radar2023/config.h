/*---Debug---*/

#define Test        // 测试标志
#define TestWithVis // 显示可视化检测结果
#define UsingVideo  // 是否使用视频(！！！可能造成OOM, 请注意设置FRAME_DEPTH)
#define ShowDepth   // 是否在GUI覆盖深度图

/*---Common settings---*/

#define lidarTopicName (char *)"/livox/lidar"

/*---For Video Record---*/

#define FRAME_DEPTH 100 // 图像队列深度，OOM时适当减少

/*---For depth and common function---*/

#define MaxPointsNum 10000 // 最大点云数量
#define ImageH 2064        // 图像高度
#define ImageW 3088        // 图像宽度
#define maxQueueSize 100   // 点云最大帧队列长度
#define LidarQueueSize 1   // 雷达消息队列
#define Epsilon (const float)1e-6

/*---For UART game info---*/

#define MAXBO 3

/*---For Location function---*/

#define Z_A true        // Z轴突变调整
#define L_P true        // 位置预测
#define Z_THRE 0.2      // Z轴突变阈值
#define Pre_Time 10     // 预测次数
#define Pre_ratio 0.1f  // 预测速度比例
#define Real_Size_W 15. // 真实宽度
#define Real_Size_H 28. // 真实高度（长）
#define IoU_THRE 0.8f   // IoU预测阈值

/*---For old SepTarget method [实验性][已废弃][谨慎使用]---*/

// #define UsePointCloudSepTarget
#define MDHistorySize 200 // 背景深度图帧队列大小，影响背景深度图密度
#define _blockSizeH 36    // 栅格大小[建议取值能被图像大小整除]
#define _blockSizeW 36    // 栅格大小
#define MTBoxRatio 1.1    // 分割框扩大比例
#define OffsetRatio 1.3   // 点云离散兼容比例
#define K_size 3          // 卷积核大小

/*---For deepsort [实验性][高性能消耗][谨慎使用]---*/

// #define UseDeepSort

/*---For ExperimentalOutput [实验数据输出][仅作识别效率记录][识别数据将混乱]---*/

// #define ExperimentalOutput     //启用实验模式输出
// #define UseOneLayerInfer //启用单层神经网络预测模式
