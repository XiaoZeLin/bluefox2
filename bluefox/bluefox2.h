#ifndef BLUEFOX2_H_
#define BLUEFOX2_H_

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "bluefox2_setting.h"

namespace bluefox2 {

//! Pixel Format
//! Defines the pixel format of the resulting image
#define idpf_auto 0             //!<The driver will decide which format to use
#define idpf_raw 1              //!<An unprocessed block of data
#define idpf_mono8 2            //!<A mono channel 8 bit
#define idpf_mono16 9           //!<A mono channel 16 bit
#define idpf_rgb888_packed 10   //!<RGB with 24 bit per pixel
#define idpf_bgr888_packed 22   //!<BGR with 24 bit per pixel

//! Camera binning mode
//! Defines valid binning modes for the camera
#define cbm_off 0           //!<No binning
#define cbm_binning_h 0x1   //!<Horizontal binning (combines 2 adjacent columns)
#define cbm_binning_v 0x2   //!<Vertical binning (combines 2 adjacent rows)
#define cbm_binning_hv 0x3  //!<Horizontal and vertical binning

//! Auto control parameters
//! Defines valid AutoControlSpeed modes
//! Auto control speed
#define acs_unavailable -1  //!<auto control parameters not available
#define acs_slow 0          //!<coverge slowly to desired value
#define acs_medium 1        //!<converge to desired value at medium speed
#define acs_fast 2          //!<converge fast to desired value

//! Dark current filter mode
//! Defines valid modes for the dark current filter
//! Operation mode of the dark current filter
#define dcfm_off 0          //!<filter is switched off
#define dcfm_on 1           //!<filter is switched on
#define dcfm_calibrate 2    //!<calculate dark current corrention image
#define correction_image 3  //!<replace captured image with the last correction image

//! Camera pixel clock
//! Defines valid camrea pixel frequencies
//! Pixel clock of the camera sensor in KHz
#define cpc_12000 12000 //!< 12 Mhz
#define cpc_20000 20000 //!< 20 Mhz
#define cpc_24000 24000 //!< 24 Mhz
#define cpc_27000 27000 //!< 27 Mhz
#define cpc_32000 32000 //!< 32 Mhz
#define cpc_40000 40000 //!< 40 Mhz
#define cpc_50000 50000 //!< 50 Mhz

//! Camera trigger mode
//! Defines valid camera sensor trigger modes
#define ctm_continuous 0       //!< continuously exposes images
#define ctm_on_demand 1        //!< start frame expose when software asks for an image
#define ctm_on_low_level 2     //!< start the exposure of a frame when the trigger input is below the trigger threshold
#define ctm_on_high_level 3    //!< start the exposure of a frame when the trigger input is above the trigger threshold
#define ctm_on_falling_edge 4  //!< start the exposure of a frame when the trigger input level changes from high to low
#define ctm_on_rising_edge 5   //!< start the exposure of a frame when the trigger input level changes from low to high
#define hard_sync -1           //!< hardware sync with master and slave (stereo only, hack)

//! Camera trigger source
//! Defined valie camera sensor trigger source values
#define cts_unavailable -1  //!<trigger mode is continuous or on_demand
#define cts_dig_in_0 0      //!<Uses digital input 0 as the source for the trigger signal
#define cts_dig_in_1 1      //!<Use digital input 1 as the source for the trigger signal

//! White balance paramter
//! An enum to set white balance paramter
#define wbp_unavailable -1    //!< not available
#define wbp_tungsten 0        //!< Tungsten
#define wbp_halogen 1         //!< Halogen
#define wbp_fluorescent 2     //!< Fluorescent
#define wbp_daylight 3        //!< Day light
#define wbp_photolight 4      //!< Photo Light
#define wbp_bluesky 5         //!< Blue Sky
#define wbp_user1 6           //!< User1
#define wbp_calibrate 10      //!< Calibrate

//! Mirror mode
//! An enum to set mirror mode
#define mm_off 0                    //!< Mirror mode off
#define mm_topdown 1                //!< Top down
#define mm_leftright 2              //!< Left right
#define mm_topdown_and_leftright 3  //!< Top down and left right

class Bluefox2 {
 public:
  explicit Bluefox2(const std::string &serial);
  ~Bluefox2();

  const std::string &serial() const { return serial_; }
  std::string product() const { return dev_->product.readS(); }
  int timeout_ms() const { return timeout_ms_; }
  void set_timeout_ms(int timeout_ms) { timeout_ms_ = timeout_ms; }

  int GetExposeUs() const;

  void OpenDevice();
  int RequestSingle() const;
  //void Configure(Bluefox2DynConfig &config);
  //bool GrabImage(sensor_msgs::Image &image_msg);

  bool GrabImage(cv::Mat &image);

  void SetMM(int mm) const;
  void SetMaster() const;
  void SetSlave() const;

  static std::string AvailableDevice();


  //std::string AvailableDevice() const;

  bool IsCtmOnDemandSupported() const;

  // Settings
  // Area of Intreset
  void SetAoi(int &width, int &height) const;

  // Pixel Format
  //! idpf_auto             The driver will decide which format to use
  //! idpf_raw              An unprocessed block of data
  //! idpf_mono8            A mono channel 8 bit
  //! idpf_mono16           A mono channel 16 bit
  //! idpf_rgb888_packed    RGB with 24 bit per pixel
  //! idpf_bgr888_packed    BGR with 24 bit per pixel
  void SetIdpf(int &idpf) const;

  //! Binning
  //! Set Camera binning mode 
  //! cbm_off            No binning
  //! cbm_binning_h      Horizontal binning (combines 2 adjacent columns)
  //! cbm_binning_v      Vertical binning (combines 2 adjacent rows)
  //! cbm_binning_hv     Horizontal and vertical binning
  void SetCbm(int &cbm) const;

  //! Gain
  //! gain_db 0.0 ~ 32.0
  void SetAgc(bool &auto_gain, double &gain_db) const;

  //! Expose
  //! us
  //! expose_us 10~100000 
  void SetAec(bool &auto_expose, int &expose_us) const;

  //! Auto Controller
  //! Auto control speed
  //! acs_unavailable       auto control parameters not available
  //! acs_slow 0            coverge slowly to desired value
  //! acs_medium 1          converge to desired value at medium speed
  //! acs_fast 2            converge fast to desired value
  //! des_gray_val 0 ~ 255
  void SetAcs(int &acs, int &des_gray_val) const;

  //! White Balance
  //! White balance paramter
  //! wbp_unavailable       not available
  //! wbp_tungsten          Tungsten
  //! wbp_halogen           Halogen
  //! wbp_fluorescent       Fluorescent
  //! wbp_daylight          Day light
  //! wbp_photolight        Photo Light
  //! wbp_bluesky           Blue Sky
  //! wbp_user1             User1
  //! wbp_calibrate         Calibrate
  //! r_gain 0.1~10 default 1
  //! g_gain 0.1~10 default 1
  //! b_gain 0.1~10 default 1
  void SetWbp(int &wbp, double &r_gain, double &g_gain, double &b_gain) const;

  //! High Dynamic Range
  void SetHdr(bool &hdr) const;

  //! Dark Current Filter
  //! dcfm_off          filter is switched off
  //! dcfm_on           filter is switched on
  //! dcfm_calibrate    calculate dark current corrention image
  //! correction_image  replace captured image with the last correction image
  void SetDcfm(int &dcfm) const;

  //! Pixel Clock
  //! cpc_12000  12 Mhz
  //! cpc_20000  20 Mhz
  //! cpc_24000  24 Mhz
  //! cpc_27000  27 Mhz
  //! cpc_32000  32 Mhz
  //! cpc_40000  40 Mhz
  //! cpc_50000  50 Mhz 
  void SetCpc(int &cpc) const;

  //! Trigger Mode
  //! ctm_continuous         continuously exposes images
  //! ctm_on_demand          start frame expose when software asks for an image
  //! ctm_on_low_level       start the exposure of a frame when the trigger input is below the trigger threshold
  //! ctm_on_high_level      start the exposure of a frame when the trigger input is above the trigger threshold
  //! ctm_on_falling_edge    start the exposure of a frame when the trigger input level changes from high to low
  //! ctm_on_rising_edge     start the exposure of a frame when the trigger input level changes from low to high
  //! hard_sync              hardware sync with master and slave (stereo only, hack)
  void SetCtm(int &ctm) const;

  //! Trigger Source
  //! cts_unavailable     trigger mode is continuous or on_demand
  //! cts_dig_in_0        Uses digital input 0 as the source for the trigger signal
  //! cts_dig_in_1        Use digital input 1 as the source for the trigger signal
  void SetCts(int &cts) const;

  //! Request
  //! Fix this to request count
  //! This is just prefilling the capture queue
  //! 0 ~ 4
  void FillCaptureQueue(int &n) const;
  void RequestImages(int n) const;
  int manuallyStartAcquisitionIfNeeded() const;
private:
  int timeout_ms_{200};
  std::string serial_;
  //Bluefox2DynConfig config_;
  mvIMPACT::acquire::Request *request_{nullptr};
  mvIMPACT::acquire::DeviceManager dev_mgr_;
  mvIMPACT::acquire::Device *dev_{nullptr};
  mvIMPACT::acquire::FunctionInterface *fi_{nullptr};
  //  mvIMPACT::acquire::Statistics *stats_{nullptr};
  mvIMPACT::acquire::SettingsBlueFOX *bf_set_{nullptr};
  mvIMPACT::acquire::ImageProcessing *img_proc_{nullptr};
  mvIMPACT::acquire::CameraSettingsBlueFOX *cam_set_{nullptr};
  mvIMPACT::acquire::SystemSettings *sys_set_{nullptr};
  mvIMPACT::acquire::InfoBlueDevice *bf_info_{nullptr};
};

}  // namespace bluefox2

#endif  // BLUEFOX2_H_
