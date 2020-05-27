#include "texture.h"
#include "CGL/color.h"
#include "drawrend.h"
#include <math.h>

namespace CGL {

Color Texture::sample(const SampleParams &sp) {
  // Part 5: this funtion chooses between nearest pixel sampling and bilinear pixel interpolation sampling 
  if (sp.psm == P_NEAREST) {
    return sample_nearest(sp.p_uv);
  }
  else {
    return sample_bilinear(sp.p_uv);
  }
}
Color Texture::sample_nearest(Vector2D uv) {
  // Part 5: Understand how this funtion has been implemented.
  // keep value of level to 0
  int level = 0;
  int w = mipmap[level].width;
  int h = mipmap[level].height;

  int X = uv.x * w;
  int Y = uv.y * h;
  if (X >= 0 && X < w && Y >= 0 && Y < h) {
    int index = 4 * (Y * w + X);

    float r = (mipmap[level].texels[index]) / 255.0f;
    float g = (mipmap[level].texels[index+1]) / 255.0f;
    float b = (mipmap[level].texels[index+2]) / 255.0f;
    return Color(r, g, b);
  }
}


//https://dsp.stackexchange.com/questions/13697/how-do-you-interpolate-between-points-in-an-image-2d-e-g-using-splines
Color Texture::sample_bilinear(Vector2D uv) {
  // Part 5: Fill this in.
  // Hint: look at sample_nearest
  int level = 0;
  int w = mipmap[level].width;
  int h = mipmap[level].height;

  float x = uv.x * w;
  float y = uv.y * h;

  float x_right = ceil(uv.x*w);
  float x_left = floor(uv.x*w);

  float y_up = ceil(uv.y*h);
  float y_down = floor(uv.y*h);

  int pixel_downleft = 4 * (y_down * w + x_left);
  int pixel_downright = 4 * (y_down * w + x_right);

  int pixel_upleft = 4 * (y_up * w + x_left);
  int pixel_upright = 4 * (y_up * w + x_right);  

  Color Color_pdl = Color((mipmap[level].texels[pixel_downleft]) / 255.0f, (mipmap[level].texels[pixel_downleft+1]) / 255.0f, (mipmap[level].texels[pixel_downleft+2]) / 255.0f);
  Color Color_pdr = Color((mipmap[level].texels[pixel_downright]) / 255.0f, (mipmap[level].texels[pixel_downright+1]) / 255.0f, (mipmap[level].texels[pixel_downright+2]) / 255.0f);
  Color Color_pul = Color((mipmap[level].texels[pixel_upleft]) / 255.0f, (mipmap[level].texels[pixel_upleft+1]) / 255.0f, (mipmap[level].texels[pixel_upleft+2]) / 255.0f);
  Color Color_pur = Color((mipmap[level].texels[pixel_upright]) / 255.0f, (mipmap[level].texels[pixel_upright+1]) / 255.0f, (mipmap[level].texels[pixel_upright+2]) / 255.0f);

  float denom = (x_right - x_left)*(y_up - y_down);
  Color inter_pdl = Color_pdl * (((x_right - x) * (y_up - y)) / denom);
  Color inter_pdr = Color_pdr * (((x - x_left)  * (y_up - y)) / denom);
  Color inter_pul = Color_pul * (((x_right - x) * (y - y_down)) / denom);
  Color inter_pur = Color_pur * (((x - x_left)  * (y - y_down)) / denom);

  Color output = inter_pdl + inter_pdr + inter_pul + inter_pur;
  return output;
}




/****************************************************************************/



inline void uint8_to_float(float dst[4], unsigned char *src) {
  uint8_t *src_uint8 = (uint8_t *)src;
  dst[0] = src_uint8[0] / 255.f;
  dst[1] = src_uint8[1] / 255.f;
  dst[2] = src_uint8[2] / 255.f;
  dst[3] = src_uint8[3] / 255.f;
}

inline void float_to_uint8(unsigned char *dst, float src[4]) {
  uint8_t *dst_uint8 = (uint8_t *)dst;
  dst_uint8[0] = (uint8_t)(255.f * max(0.0f, min(1.0f, src[0])));
  dst_uint8[1] = (uint8_t)(255.f * max(0.0f, min(1.0f, src[1])));
  dst_uint8[2] = (uint8_t)(255.f * max(0.0f, min(1.0f, src[2])));
  dst_uint8[3] = (uint8_t)(255.f * max(0.0f, min(1.0f, src[3])));
}

void Texture::generate_mips(int startLevel) {

  // make sure there's a valid texture
  if (startLevel >= mipmap.size()) {
    std::cerr << "Invalid start level";
  }

  // allocate sublevels
  int baseWidth = mipmap[startLevel].width;
  int baseHeight = mipmap[startLevel].height;
  int numSubLevels = (int)(log2f((float)max(baseWidth, baseHeight)));

  numSubLevels = min(numSubLevels, kMaxMipLevels - startLevel - 1);
  mipmap.resize(startLevel + numSubLevels + 1);

  int width = baseWidth;
  int height = baseHeight;
  for (int i = 1; i <= numSubLevels; i++) {

    MipLevel &level = mipmap[startLevel + i];

    // handle odd size texture by rounding down
    width = max(1, width / 2);
    //assert (width > 0);
    height = max(1, height / 2);
    //assert (height > 0);

    level.width = width;
    level.height = height;
    level.texels = vector<unsigned char>(4 * width * height);
  }

  // create mips
  int subLevels = numSubLevels - (startLevel + 1);
  for (int mipLevel = startLevel + 1; mipLevel < startLevel + subLevels + 1;
       mipLevel++) {

    MipLevel &prevLevel = mipmap[mipLevel - 1];
    MipLevel &currLevel = mipmap[mipLevel];

    int prevLevelPitch = prevLevel.width * 4; // 32 bit RGBA
    int currLevelPitch = currLevel.width * 4; // 32 bit RGBA

    unsigned char *prevLevelMem;
    unsigned char *currLevelMem;

    currLevelMem = (unsigned char *)&currLevel.texels[0];
    prevLevelMem = (unsigned char *)&prevLevel.texels[0];

    float wDecimal, wNorm, wWeight[3];
    int wSupport;
    float hDecimal, hNorm, hWeight[3];
    int hSupport;

    float result[4];
    float input[4];

    // conditional differentiates no rounding case from round down case
    if (prevLevel.width & 1) {
      wSupport = 3;
      wDecimal = 1.0f / (float)currLevel.width;
    } else {
      wSupport = 2;
      wDecimal = 0.0f;
    }

    // conditional differentiates no rounding case from round down case
    if (prevLevel.height & 1) {
      hSupport = 3;
      hDecimal = 1.0f / (float)currLevel.height;
    } else {
      hSupport = 2;
      hDecimal = 0.0f;
    }

    wNorm = 1.0f / (2.0f + wDecimal);
    hNorm = 1.0f / (2.0f + hDecimal);

    // case 1: reduction only in horizontal size (vertical size is 1)
    if (currLevel.height == prevLevel.height) {
      //assert (currLevel.height == 1);

      for (int i = 0; i < currLevel.width; i++) {
        wWeight[0] = wNorm * (1.0f - wDecimal * i);
        wWeight[1] = wNorm * 1.0f;
        wWeight[2] = wNorm * wDecimal * (i + 1);

        result[0] = result[1] = result[2] = result[3] = 0.0f;

        for (int ii = 0; ii < wSupport; ii++) {
          uint8_to_float(input, prevLevelMem + 4 * (2 * i + ii));
          result[0] += wWeight[ii] * input[0];
          result[1] += wWeight[ii] * input[1];
          result[2] += wWeight[ii] * input[2];
          result[3] += wWeight[ii] * input[3];
        }

        // convert back to format of the texture
        float_to_uint8(currLevelMem + (4 * i), result);
      }

      // case 2: reduction only in vertical size (horizontal size is 1)
    } else if (currLevel.width == prevLevel.width) {
      //assert (currLevel.width == 1);

      for (int j = 0; j < currLevel.height; j++) {
        hWeight[0] = hNorm * (1.0f - hDecimal * j);
        hWeight[1] = hNorm;
        hWeight[2] = hNorm * hDecimal * (j + 1);

        result[0] = result[1] = result[2] = result[3] = 0.0f;
        for (int jj = 0; jj < hSupport; jj++) {
          uint8_to_float(input, prevLevelMem + prevLevelPitch * (2 * j + jj));
          result[0] += hWeight[jj] * input[0];
          result[1] += hWeight[jj] * input[1];
          result[2] += hWeight[jj] * input[2];
          result[3] += hWeight[jj] * input[3];
        }

        // convert back to format of the texture
        float_to_uint8(currLevelMem + (currLevelPitch * j), result);
      }

      // case 3: reduction in both horizontal and vertical size
    } else {

      for (int j = 0; j < currLevel.height; j++) {
        hWeight[0] = hNorm * (1.0f - hDecimal * j);
        hWeight[1] = hNorm;
        hWeight[2] = hNorm * hDecimal * (j + 1);

        for (int i = 0; i < currLevel.width; i++) {
          wWeight[0] = wNorm * (1.0f - wDecimal * i);
          wWeight[1] = wNorm * 1.0f;
          wWeight[2] = wNorm * wDecimal * (i + 1);

          result[0] = result[1] = result[2] = result[3] = 0.0f;

          // convolve source image with a trapezoidal filter.
          // in the case of no rounding this is just a box filter of width 2.
          // in the general case, the support region is 3x3.
          for (int jj = 0; jj < hSupport; jj++)
            for (int ii = 0; ii < wSupport; ii++) {
              float weight = hWeight[jj] * wWeight[ii];
              uint8_to_float(input, prevLevelMem +
                                        prevLevelPitch * (2 * j + jj) +
                                        4 * (2 * i + ii));
              result[0] += weight * input[0];
              result[1] += weight * input[1];
              result[2] += weight * input[2];
              result[3] += weight * input[3];
            }

          // convert back to format of the texture
          float_to_uint8(currLevelMem + currLevelPitch * j + 4 * i, result);
        }
      }
    }
  }
}

}
