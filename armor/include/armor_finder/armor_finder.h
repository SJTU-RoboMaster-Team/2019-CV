//
// Created by xinyang on 19-3-27.
//

#ifndef _ARMOR_FINDER_H_
#define _ARMOR_FINDER_H_

#include <map>
#include <systime.h>
#include <constants.h>
#include <opencv2/core.hpp>
#include <opencv2/tracking.hpp>
#include <serial.h>
#include <armor_finder/classifier/classifier.h>
#include <additions.h>

#define BLOB_RED    ENEMY_RED
#define BLOB_BLUE   ENEMY_BLUE

#define BOX_RED     ENEMY_RED
#define BOX_BLUE    ENEMY_BLUE

#define IMAGE_CENTER_X      (320)
#define IMAGE_CENTER_Y      (240-20)

#define DISTANCE_HEIGHT_5MM (10700.0)     // 单位: cm*pixel
#define DISTANCE_HEIGHT     DISTANCE_HEIGHT_5MM

#define         B1 1
#define         B2 2
#define         B3 3
#define         B4 4
#define         B5 5
#define         B7 6
#define         B8 7
#define         R1 8
#define         R2 9
#define         R3 10
#define         R4 11
#define         R5 12
#define         R7 13
#define         R8 14

extern std::map<int, string> id2name;   //装甲板id到名称的map
extern std::map<string, int> name2id;   //装甲板名称到id的map
extern std::map<string, int> prior_blue;
extern std::map<string, int> prior_red;


/******************* 灯条类定义 ***********************/
class LightBlob {
public:
    cv::RotatedRect rect;   //灯条位置
    double area_ratio;
    double length;          //灯条长度
    uint8_t blob_color;      //灯条颜色

    LightBlob(cv::RotatedRect &r, double ratio, uint8_t color) : rect(r), area_ratio(ratio), blob_color(color) {
        length = max(rect.size.height, rect.size.width);
    };
    LightBlob() = default;
};

typedef std::vector<LightBlob> LightBlobs;



/******************* 装甲板类定义　**********************/
class ArmorBox{
public:
    typedef enum{
        FRONT, SIDE, UNKNOWN
    } BoxOrientation;

    cv::Rect2d rect;
    LightBlobs light_blobs;
    uint8_t box_color;
    int id;

    explicit ArmorBox(const cv::Rect &pos=cv::Rect2d(), const LightBlobs &blobs=LightBlobs(), uint8_t color=0, int i=0);

    cv::Point2f getCenter() const; // 获取装甲板中心
    double getBlobsDistance() const; // 获取两个灯条中心间距
    double lengthDistanceRatio() const; // 获取灯条中心距和灯条长度的比值
    double getBoxDistance() const; // 获取装甲板到摄像头的距离
    BoxOrientation getOrientation() const; // 获取装甲板朝向(误差较大，已弃用)

    bool operator<(const ArmorBox &box) const; // 装甲板优先级比较
};

typedef std::vector<ArmorBox> ArmorBoxes;

/********************* 自瞄类定义 **********************/
class ArmorFinder{
public:
    ArmorFinder(uint8_t &color, Serial &u, const string &paras_folder, const uint8_t &anti_top);
    ~ArmorFinder() = default;

private:
    typedef cv::TrackerKCF TrackerToUse;                // Tracker类型定义

    typedef enum{
        SEARCHING_STATE, TRACKING_STATE, STANDBY_STATE
    } State;                                            // 自瞄状态枚举定义

    systime frame_time;                                 // 当前帧对应时间
    const uint8_t &enemy_color;                         // 敌方颜色，引用外部变量，自动变化
    const uint8_t &is_anti_top;                         // 进入反陀螺，引用外部变量，自动变化
    State state;                                        // 自瞄状态对象实例
    ArmorBox target_box, last_box;                      // 目标装甲板
    int anti_switch_cnt;                                // 防止乱切目标计数器
    cv::Ptr<cv::Tracker> tracker;                       // tracker对象实例
    Classifier classifier;                              // CNN分类器对象实例，用于数字识别
    int contour_area;                                   // 装甲区域亮点个数，用于数字识别未启用时判断是否跟丢（已弃用）
    int tracking_cnt;                                   // 记录追踪帧数，用于定时退出追踪
    Serial &serial;                                     // 串口对象，引用外部变量，用于和能量机关共享同一个变量
    systime last_front_time;                            // 上次陀螺正对时间
    int anti_top_cnt;
    RoundQueue<double, 4> top_periodms;                 // 陀螺周期循环队列
    vector<systime> time_seq;                           // 一个周期内的时间采样点
    vector<float> angle_seq;                            // 一个周期内的角度采样点

    bool findLightBlobs(const cv::Mat &src, LightBlobs &light_blobs);
    bool findArmorBox(const cv::Mat &src, ArmorBox &box);
    bool matchArmorBoxes(const cv::Mat &src, const LightBlobs &light_blobs, ArmorBoxes &armor_boxes);

    bool stateSearchingTarget(cv::Mat &src);            // searching state主函数
    bool stateTrackingTarget(cv::Mat &src);             // tracking state主函数
    bool stateStandBy();                                // stand by state主函数（已弃用）

    void antiTop();                                     // 反小陀螺

    bool sendBoxPosition(uint16_t shoot);               // 和主控板通讯
public:
    void run(cv::Mat &src);                             // 自瞄主函数
};

#endif /* _ARMOR_FINDER_H_ */
