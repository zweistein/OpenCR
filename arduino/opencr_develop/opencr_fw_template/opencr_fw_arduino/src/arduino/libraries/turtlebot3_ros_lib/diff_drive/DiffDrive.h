#ifndef _ROS_diff_drive_DiffDrive_h
#define _ROS_diff_drive_DiffDrive_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace diff_drive
{

  class DiffDrive : public ros::Msg
  {
    public:
      typedef float _Left_type;
      _Left_type Left;
      typedef float _Right_type;
      _Right_type Right;

    DiffDrive():
      Left(0),
      Right(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      union {
        float real;
        uint32_t base;
      } u_Left;
      u_Left.real = this->Left;
      *(outbuffer + offset + 0) = (u_Left.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_Left.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_Left.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_Left.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->Left);
      union {
        float real;
        uint32_t base;
      } u_Right;
      u_Right.real = this->Right;
      *(outbuffer + offset + 0) = (u_Right.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_Right.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_Right.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_Right.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->Right);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      union {
        float real;
        uint32_t base;
      } u_Left;
      u_Left.base = 0;
      u_Left.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_Left.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_Left.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_Left.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->Left = u_Left.real;
      offset += sizeof(this->Left);
      union {
        float real;
        uint32_t base;
      } u_Right;
      u_Right.base = 0;
      u_Right.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_Right.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_Right.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_Right.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->Right = u_Right.real;
      offset += sizeof(this->Right);
     return offset;
    }

    const char * getType(){ return "diff_drive/DiffDrive"; };
    const char * getMD5(){ return "4439436cf2307c11d1604e4703116c09"; };

  };

}
#endif