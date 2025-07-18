#include "pch.h"
#include "BlazeEngineCore/Math/Random.h"
#include <random>

namespace Blaze::Random
{
    static const uint8_t perm[256] = {
        151, 160, 137, 91, 90, 15,
        131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
        190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
        88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
        77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
        102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
        135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
        5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
        223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
        129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
        251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
        49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
        138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
    };
    static inline uint8_t hash(int32_t i) {
        return perm[static_cast<uint8_t>(i)];
    }  
    static inline uint8_t hash(int64_t i) {
        return perm[static_cast<uint8_t>(i)];
    }

    static inline int32_t fastfloor_float(float fp) {
        int32_t i = static_cast<int32_t>(fp);
        return (fp < i) ? (i - 1) : (i);
    }
    static float grad_float(int32_t hash, float x) {
        const int32_t h = hash & 0x0F;  
        float grad_float = 1.0f + (h & 7);    
        if ((h & 8) != 0) grad_float = -grad_float;     
        return (grad_float * x);              
    }
    static float grad_float(int32_t hash, float x, float y) {
        const int32_t h = hash & 0x3F;  
        const float u = h < 4 ? x : y;  
        const float v = h < 4 ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
    }
    static float grad_float(int32_t hash, float x, float y, float z) {
        int h = hash & 15;    
        float u = h < 8 ? x : y; 
        float v = h < 4 ? y : h == 12 || h == 14 ? x : z; 
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    }

    static inline int64_t fastfloor_double(double fp) {
        int64_t i = static_cast<int64_t>(fp);
        return (fp < i) ? (i - 1) : (i);
    }
    static double grad_double(int64_t hash, double x) {
        const int64_t h = hash & 0x0F;
        double grad_double = 1.0 + (h & 7);
        if ((h & 8) != 0) grad_double = -grad_double;
        return (grad_double * x);
    }
    static double grad_double(int64_t hash, double x, double y) {
        const int64_t h = hash & 0x3F;
        const double u = h < 4 ? x : y;
        const double v = h < 4 ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -2.0 * v : 2.0 * v);
    }
    static double grad_double(int64_t hash, double x, double y, double z) {
        int h = hash & 15;
        double u = h < 8 ? x : y;
        double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    }
    
    void SetSeed(uint seed)
    {
        srand(seed);
    }

    float Float()
    {
        return ((float)rand()) / (float)RAND_MAX;        
    }

    float Float(float min, float max)
    {
        return ((float)rand()) * (max - min) / (float)RAND_MAX + min;
    }    

    float Simplex(float x) {
        float n0, n1;
        
        int32_t i0 = fastfloor_float(x);
        int32_t i1 = i0 + 1;
        
        float x0 = x - i0;
        float x1 = x0 - 1.0f;
        
        float t0 = 1.0f - x0 * x0;        
        t0 *= t0;
        n0 = t0 * t0 * grad_float(hash(i0), x0);
        
        float t1 = 1.0f - x1 * x1;        
        t1 *= t1;
        n1 = t1 * t1 * grad_float(hash(i1), x1);        
        return 0.395f * (n0 + n1);
    }
    float Simplex(Vec2f v) {
        float n0, n1, n2;   
        
        static const float F2 = 0.366025403f; 
        static const float G2 = 0.211324865f; 
        
        const float s = (v.x + v.y) * F2; 
        const float xs = v.x + s;
        const float ys = v.y + s;
        const int32_t i = fastfloor_float(xs);
        const int32_t j = fastfloor_float(ys);
        
        const float t = static_cast<float>(i + j) * G2;
        const float X0 = i - t;
        const float Y0 = j - t;
        const float x0 = v.x - X0; 
        const float y0 = v.y - Y0;
        
        int32_t i1, j1;  
        if (x0 > y0) {   
            i1 = 1;
            j1 = 0;
        }
        else {   
            i1 = 0;
            j1 = 1;
        }

        const float x1 = x0 - i1 + G2;           
        const float y1 = y0 - j1 + G2;
        const float x2 = x0 - 1.0f + 2.0f * G2;  
        const float y2 = y0 - 1.0f + 2.0f * G2;
        
        const int gi0 = hash(i + hash(j));
        const int gi1 = hash(i + i1 + hash(j + j1));
        const int gi2 = hash(i + 1 + hash(j + 1));
        
        float t0 = 0.5f - x0 * x0 - y0 * y0;
        if (t0 < 0.0f) {
            n0 = 0.0f;
        }
        else {
            t0 *= t0;
            n0 = t0 * t0 * grad_float(gi0, x0, y0);
        }
        
        float t1 = 0.5f - x1 * x1 - y1 * y1;
        if (t1 < 0.0f) {
            n1 = 0.0f;
        }
        else {
            t1 *= t1;
            n1 = t1 * t1 * grad_float(gi1, x1, y1);
        }
        
        float t2 = 0.5f - x2 * x2 - y2 * y2;
        if (t2 < 0.0f) {
            n2 = 0.0f;
        }
        else {
            t2 *= t2;
            n2 = t2 * t2 * grad_float(gi2, x2, y2);
        }
        
        return 45.23065f * (n0 + n1 + n2);
    }
    float Simplex(Vec3f v) {
        float n0, n1, n2, n3; 
        
        static const float F3 = 1.0f / 3.0f;
        static const float G3 = 1.0f / 6.0f;
        
        float s = (v.x + v.y + v.z) * F3; 
        int i = fastfloor_float(v.x + s);
        int j = fastfloor_float(v.y + s);
        int k = fastfloor_float(v.z + s);
        float t = (i + j + k) * G3;
        float X0 = i - t;
        float Y0 = j - t;
        float Z0 = k - t;
        float x0 = v.x - X0; 
        float y0 = v.y - Y0;
        float z0 = v.z - Z0;
        
        int i1, j1, k1; 
        int i2, j2, k2; 
        if (x0 >= y0) {
            if (y0 >= z0) {
                i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0; 
            }
            else if (x0 >= z0) {
                i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; 
            }
            else {
                i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; 
            }
        }
        else { 
            if (y0 < z0) {
                i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; 
            }
            else if (x0 < z0) {
                i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; 
            }
            else {
                i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; 
            }
        }

        float x1 = x0 - i1 + G3; 
        float y1 = y0 - j1 + G3;
        float z1 = z0 - k1 + G3;
        float x2 = x0 - i2 + 2.0f * G3; 
        float y2 = y0 - j2 + 2.0f * G3;
        float z2 = z0 - k2 + 2.0f * G3;
        float x3 = x0 - 1.0f + 3.0f * G3; 
        float y3 = y0 - 1.0f + 3.0f * G3;
        float z3 = z0 - 1.0f + 3.0f * G3;
        
        int gi0 = hash(i + hash(j + hash(k)));
        int gi1 = hash(i + i1 + hash(j + j1 + hash(k + k1)));
        int gi2 = hash(i + i2 + hash(j + j2 + hash(k + k2)));
        int gi3 = hash(i + 1 + hash(j + 1 + hash(k + 1)));
        
        float t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;
        if (t0 < 0) {
            n0 = 0.0;
        }
        else {
            t0 *= t0;
            n0 = t0 * t0 * grad_float(gi0, x0, y0, z0);
        }
        float t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
        if (t1 < 0) {
            n1 = 0.0;
        }
        else {
            t1 *= t1;
            n1 = t1 * t1 * grad_float(gi1, x1, y1, z1);
        }
        float t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;
        if (t2 < 0) {
            n2 = 0.0;
        }
        else {
            t2 *= t2;
            n2 = t2 * t2 * grad_float(gi2, x2, y2, z2);
        }
        float t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;
        if (t3 < 0) {
            n3 = 0.0;
        }
        else {
            t3 *= t3;
            n3 = t3 * t3 * grad_float(gi3, x3, y3, z3);
        }        
        return 32.0f * (n0 + n1 + n2 + n3);
    }

    double Simplex(double x) {
        double n0, n1;

        int64_t i0 = fastfloor_double(x);
        int64_t i1 = i0 + 1;

        double x0 = x - i0;
        double x1 = x0 - 1.0f;

        double t0 = 1.0f - x0 * x0;
        t0 *= t0;
        n0 = t0 * t0 * grad_double(hash(i0), x0);

        double t1 = 1.0f - x1 * x1;
        t1 *= t1;
        n1 = t1 * t1 * grad_double(hash(i1), x1);
        return 0.395f * (n0 + n1);
    }
    double Simplex(Vec2d v) {
        double n0, n1, n2;

        static const double F2 = 0.366025403f;
        static const double G2 = 0.211324865f;

        const double s = (v.x + v.y) * F2;
        const double xs = v.x + s;
        const double ys = v.y + s;
        const int64_t i = fastfloor_double(xs);
        const int64_t j = fastfloor_double(ys);

        const double t = static_cast<double>(i + j) * G2;
        const double X0 = i - t;
        const double Y0 = j - t;
        const double x0 = v.x - X0;
        const double y0 = v.y - Y0;

        int64_t i1, j1;
        if (x0 > y0) {
            i1 = 1;
            j1 = 0;
        }
        else {
            i1 = 0;
            j1 = 1;
        }

        const double x1 = x0 - i1 + G2;
        const double y1 = y0 - j1 + G2;
        const double x2 = x0 - 1.0f + 2.0f * G2;
        const double y2 = y0 - 1.0f + 2.0f * G2;

        const int gi0 = hash(i + hash(j));
        const int gi1 = hash(i + i1 + hash(j + j1));
        const int gi2 = hash(i + 1 + hash(j + 1));

        double t0 = 0.5f - x0 * x0 - y0 * y0;
        if (t0 < 0.0f) {
            n0 = 0.0f;
        }
        else {
            t0 *= t0;
            n0 = t0 * t0 * grad_double(gi0, x0, y0);
        }

        double t1 = 0.5f - x1 * x1 - y1 * y1;
        if (t1 < 0.0f) {
            n1 = 0.0f;
        }
        else {
            t1 *= t1;
            n1 = t1 * t1 * grad_double(gi1, x1, y1);
        }

        double t2 = 0.5f - x2 * x2 - y2 * y2;
        if (t2 < 0.0f) {
            n2 = 0.0f;
        }
        else {
            t2 *= t2;
            n2 = t2 * t2 * grad_double(gi2, x2, y2);
        }

        return 45.23065f * (n0 + n1 + n2);
    }
    double Simplex(Vec3d v) {
        double n0, n1, n2, n3;

        static const double F3 = 1.0f / 3.0f;
        static const double G3 = 1.0f / 6.0f;

        double s = (v.x + v.y + v.z) * F3;
        int64 i = fastfloor_double(v.x + s);
        int64 j = fastfloor_double(v.y + s);
        int64 k = fastfloor_double(v.z + s);
        double t = (i + j + k) * G3;
        double X0 = i - t;
        double Y0 = j - t;
        double Z0 = k - t;
        double x0 = v.x - X0;
        double y0 = v.y - Y0;
        double z0 = v.z - Z0;

        int i1, j1, k1;
        int i2, j2, k2;
        if (x0 >= y0) {
            if (y0 >= z0) {
                i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
            }
            else if (x0 >= z0) {
                i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1;
            }
            else {
                i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1;
            }
        }
        else {
            if (y0 < z0) {
                i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1;
            }
            else if (x0 < z0) {
                i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1;
            }
            else {
                i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
            }
        }

        double x1 = x0 - i1 + G3;
        double y1 = y0 - j1 + G3;
        double z1 = z0 - k1 + G3;
        double x2 = x0 - i2 + 2.0f * G3;
        double y2 = y0 - j2 + 2.0f * G3;
        double z2 = z0 - k2 + 2.0f * G3;
        double x3 = x0 - 1.0f + 3.0f * G3;
        double y3 = y0 - 1.0f + 3.0f * G3;
        double z3 = z0 - 1.0f + 3.0f * G3;

        int gi0 = hash(i + hash(j + hash(k)));
        int gi1 = hash(i + i1 + hash(j + j1 + hash(k + k1)));
        int gi2 = hash(i + i2 + hash(j + j2 + hash(k + k2)));
        int gi3 = hash(i + 1 + hash(j + 1 + hash(k + 1)));

        double t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;
        if (t0 < 0) {
            n0 = 0.0;
        }
        else {
            t0 *= t0;
            n0 = t0 * t0 * grad_double(gi0, x0, y0, z0);
        }
        double t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
        if (t1 < 0) {
            n1 = 0.0;
        }
        else {
            t1 *= t1;
            n1 = t1 * t1 * grad_double(gi1, x1, y1, z1);
        }
        double t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;
        if (t2 < 0) {
            n2 = 0.0;
        }
        else {
            t2 *= t2;
            n2 = t2 * t2 * grad_double(gi2, x2, y2, z2);
        }
        double t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;
        if (t3 < 0) {
            n3 = 0.0;
        }
        else {
            t3 *= t3;
            n3 = t3 * t3 * grad_double(gi3, x3, y3, z3);
        }
        return 32.0f * (n0 + n1 + n2 + n3);
    }

    Blaze::ColorRGB ColorRGB()
    {
        return Blaze::ColorRGB(static_cast<uint8>(rand()), static_cast<uint8>(rand()), static_cast<uint8>(rand()));
    }
    Blaze::ColorRGBf ColorRGBf()
    {
        return Blaze::ColorRGBf(Float(0, 1), Float(0, 1), Float(0, 1));
    }
    Blaze::ColorRGBA ColorRGBA()
    {
        return Blaze::ColorRGBA(static_cast<uint8>(rand()), static_cast<uint8>(rand()), static_cast<uint8>(rand()), 255);
    }
    Blaze::ColorRGBAf ColorRGBAf()
    {
        return Blaze::ColorRGBAf(Float(0, 1), Float(0, 1), Float(0, 1), 1);
    }
}