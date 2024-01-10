#ifndef __MAPMAPPING_H
#define __MAPMAPPING_H

#include "../../Common/include/algorithm.h"
#include "../../Detectors/include/depthProcesser.h"

/**
 * @brief 映射类
 * 使用四点标定所得旋转平移向量进行坐标系转换
 */
class MapMapping
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef std::shared_ptr<MapMapping> Ptr;  
    map<int, int> _ids = {{0, 6}, {1, 7}, {2, 8}, {3, 9}, {4, 10}, {5, 11}, {6, 0}, {7, 1}, {8, 2}, {9, 3}, {10, 4}, {11, 5}};

private:
    vector<MapLocation3D> _location3D;
    vector<MapLocation3D> cached_location3D;

    Matrix<float, 4, 4> _T;
    Matrix<float, 4, 4> T;
    Matrix<float, 3, 1> cameraPostion;
    Mat rvec, tvec;
    bool _pass_flag = false;

    int _location_pred_time[12] = {0};
    vector<vector<MapLocation3D>> _location_cache;
    vector<bboxAndRect> _IoU_pred_cache;
#if defined UseDeepSort && !(defined UsePointCloudSepTarget)
    map<int, int> _DeepSort_pred_cache; //classid, trackid
#endif
    std::shared_ptr<spdlog::logger> logger = spdlog::get("RadarLogger");

private:
    void adjust_z_one(MapLocation3D &locs);
    void _location_prediction();

public:
    MapMapping();
    ~MapMapping();

    bool _is_pass();
    void push_T(Mat &rvec, Mat &tvec);
    void _plot_region_rect(vector<vector<Point3f>> &points, Mat &frame, Mat &K_0, Mat &C_0);
    vector<ArmorBoundingBox> _IoU_prediction(vector<bboxAndRect> pred, vector<DetectBox> sepboxs);
#if defined UseDeepSort && !(defined UsePointCloudSepTarget)
    void _DeepSort_prediction(vector<bboxAndRect> &pred, vector<DetectBox> sepboxs);
#endif
    vector<MapLocation3D> getloc();
    void mergeUpdata(vector<bboxAndRect> &pred, vector<ArmorBoundingBox> &Ioubbox, Mat &K_0, Mat &C_0);
};

#endif