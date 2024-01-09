#ifndef __RADAR_H
#define __RADAR_H

#include "../../Common/include/public.h"
#include "../../Camera/include/camera.h"
#include "../../Camera/include/VideoRecorder.h"
#include "../../Common/include/SharedQueue.h"
#include "../../Detectors/include/depthProcesser.h"
#include "../../Detectors/include/ArmorDetector.h"

#ifdef UsePointCloudSepTarget
#include "../../Detectors/include/MovementDetector.h"
#else
#include "../../Detectors/include/CarDetector.h"
#endif

#if defined UseDeepSort && !(defined UsePointCloudSepTarget)
#include "../../DsTracker/include/DsTracker.h"
#endif

#include "../../Location/include/MapMapping.h"
#include "../../UART/include/UART.h"
#include "../../Location/include/location.h"

#ifdef ExperimentalOutput
#include "../../Logger/include/ExpLog.h"
#endif

#include <radar2023/Locations.h>
#include <radar2023/Location.h>

/**
 * @brief 主要雷达类
 * 负责相关工作线程的管理，获取雷达点云数据并进行处理
 */
class Radar
{
public:
    typedef std::shared_ptr<Radar> Ptr;

private:
    string share_path;
    int ENEMY;
    string CAMERA_PARAM;
    string PASSWORD;
    string OnnxForArmor;
    string EngineForArmor;
    string CameraConfig;
    string SerialPortName;

    bool ExitProgramSiginal = false;
    bool VideoRecorderSiginal = true;

#ifdef ShowDepth
    bool _if_coverDepth = true;
#endif

#ifdef UsingVideo
    string TestVideo;
#endif

    bool _is_LidarInited = false;
    std::shared_ptr<ros::NodeHandle> nh;
    std::shared_ptr<image_transport::ImageTransport> image_transport;
    ros::Subscriber sub_lidar;
    image_transport::Publisher GUI_image_pub_;
    ros::Publisher pub_locations;

    thread rosSpinLoop;
    thread serRead;
    thread serWrite;
    thread processLoop;
    thread videoRecoderLoop;
    bool _init_flag = false;
    bool _thread_working = false;
    bool _Ser_working = false;
    bool _CameraThread_working = false;
    bool __RosSpinLoop_working = false;
    bool __MainProcessLoop_working = false;
    bool __VideoRecorderLoop_working = false;
    bool _recorder_block = false;

    DepthQueue::Ptr depthQueue;
    ArmorDetector::Ptr armorDetector;

#if !(defined UseOneLayerInfer)
#ifdef UsePointCloudSepTarget
    MovementDetector::Ptr movementDetector;
#else
    string OnnxForCar;
    string EngineForCar;
    CarDetector::Ptr carDetector;
#endif
#endif

#if defined UseDeepSort && !(defined UsePointCloudSepTarget)
    string OnnxForSort;
    string EngineForSort;
    DsTracker::Ptr dsTracker;
#endif

#ifdef ExperimentalOutput
    ExpLog::Ptr myExpLog;
#endif

    CameraThread::Ptr cameraThread;
    Location::Ptr myLocation;
    MapMapping::Ptr mapMapping;
    UART::Ptr myUART;
    MySerial::Ptr mySerial;
    VideoRecorder::Ptr videoRecorder;

    bool _if_record = false;
    bool is_alive = true;

    vector<vector<float>> publicDepth;
    shared_timed_mutex myMutex_publicDepth;
    shared_timed_mutex myMutex_cameraThread;
    SharedQueue<Mat> myFrames;

    Mat K_0_Mat;
    Mat C_0_Mat;
    Mat E_0_Mat;

    std::shared_ptr<spdlog::logger> logger = spdlog::get("RadarLogger");

    vector<vector<Point3f>> show_region = vector<vector<Point3f>>{{Point3f(15.682, 14.844 - 15.f, 0.3f), Point3f(23.464, 14.844 - 15.f, 0.3f), Point3f(23.464, 13.984 - 15.f, 0.3f), Point3f(15.682, 13.984 - 15.f, 0.3f)},
                                                                  {Point3f(4.536, 1.016 - 15.f, 0.3f), Point3f(12.318, 1.016 - 15.f, 0.3f), Point3f(12.318, 0.156 - 15.f, 0.3f), Point3f(4.536, 0.156 - 15.f, 0.3f)}};

    const int ids[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

private:
    void armor_filter(vector<bboxAndRect> &pred);
    void detectDepth(vector<bboxAndRect> &pred);
    void detectDepth(vector<ArmorBoundingBox> &armors);
    void send_judge(judge_message &message);

    void drawBbox(vector<DetectBox> &bboxs, Mat &img);
    void drawArmorsForDebug(vector<ArmorBoundingBox> &armors, Mat &img);
    void drawArmorsForDebug(vector<bboxAndRect> &armors, Mat &img);

public:
    Radar();
    ~Radar();

    void init();

    void setRosNodeHandle(ros::NodeHandle &nh);
    void setRosImageTransport(image_transport::ImageTransport &image_transport);
    void setRosPackageSharedPath(String &path);

    void LidarListenerBegin();
    void RosSpinLoop();
    void LidarCallBack(const sensor_msgs::PointCloud2::ConstPtr &msg);
    void SerReadLoop();
    void SerWriteLoop();
    void MainProcessLoop();
    void VideoRecorderLoop();

    void spin();
    void stop();

    bool alive();
};

#endif