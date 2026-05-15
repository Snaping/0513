using System;

namespace ThreeDVisualizer
{
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
        }

        public static Vector3 operator -(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
        }

        public static Vector3 operator *(Vector3 v, float s)
        {
            return new Vector3(v.X * s, v.Y * s, v.Z * s);
        }

        public float Length()
        {
            return (float)Math.Sqrt(X * X + Y * Y + Z * Z);
        }

        public Vector3 Normalize()
        {
            float len = Length();
            if (len > 0)
                return new Vector3(X / len, Y / len, Z / len);
            return this;
        }

        public static float Dot(Vector3 a, Vector3 b)
        {
            return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
        }

        public static Vector3 Cross(Vector3 a, Vector3 b)
        {
            return new Vector3(
                a.Y * b.Z - a.Z * b.Y,
                a.Z * b.X - a.X * b.Z,
                a.X * b.Y - a.Y * b.X
            );
        }
    }

    public struct Matrix4
    {
        public float[,] M;

        public Matrix4(float m00, float m01, float m02, float m03,
                       float m10, float m11, float m12, float m13,
                       float m20, float m21, float m22, float m23,
                       float m30, float m31, float m32, float m33)
        {
            M = new float[4, 4];
            M[0, 0] = m00; M[0, 1] = m01; M[0, 2] = m02; M[0, 3] = m03;
            M[1, 0] = m10; M[1, 1] = m11; M[1, 2] = m12; M[1, 3] = m13;
            M[2, 0] = m20; M[2, 1] = m21; M[2, 2] = m22; M[2, 3] = m23;
            M[3, 0] = m30; M[3, 1] = m31; M[3, 2] = m32; M[3, 3] = m33;
        }

        public static Matrix4 Identity()
        {
            return new Matrix4(
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            );
        }

        public static Matrix4 RotateX(float angle)
        {
            float cos = (float)Math.Cos(angle);
            float sin = (float)Math.Sin(angle);
            return new Matrix4(
                1, 0, 0, 0,
                0, cos, -sin, 0,
                0, sin, cos, 0,
                0, 0, 0, 1
            );
        }

        public static Matrix4 RotateY(float angle)
        {
            float cos = (float)Math.Cos(angle);
            float sin = (float)Math.Sin(angle);
            return new Matrix4(
                cos, 0, sin, 0,
                0, 1, 0, 0,
                -sin, 0, cos, 0,
                0, 0, 0, 1
            );
        }

        public static Matrix4 RotateZ(float angle)
        {
            float cos = (float)Math.Cos(angle);
            float sin = (float)Math.Sin(angle);
            return new Matrix4(
                cos, -sin, 0, 0,
                sin, cos, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            );
        }

        public static Matrix4 Translate(float x, float y, float z)
        {
            return new Matrix4(
                1, 0, 0, x,
                0, 1, 0, y,
                0, 0, 1, z,
                0, 0, 0, 1
            );
        }

        public static Vector3 operator *(Matrix4 m, Vector3 v)
        {
            float w = m.M[3, 0] * v.X + m.M[3, 1] * v.Y + m.M[3, 2] * v.Z + m.M[3, 3];
            return new Vector3(
                (m.M[0, 0] * v.X + m.M[0, 1] * v.Y + m.M[0, 2] * v.Z + m.M[0, 3]) / w,
                (m.M[1, 0] * v.X + m.M[1, 1] * v.Y + m.M[1, 2] * v.Z + m.M[1, 3]) / w,
                (m.M[2, 0] * v.X + m.M[2, 1] * v.Y + m.M[2, 2] * v.Z + m.M[2, 3]) / w
            );
        }

        public static Matrix4 operator *(Matrix4 a, Matrix4 b)
        {
            Matrix4 result = Identity();
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    result.M[i, j] = 0;
                    for (int k = 0; k < 4; k++)
                    {
                        result.M[i, j] += a.M[i, k] * b.M[k, j];
                    }
                }
            }
            return result;
        }

        public static Matrix4 Perspective(float fov, float aspect, float near, float far)
        {
            float f = 1f / (float)Math.Tan(fov / 2);
            return new Matrix4(
                f / aspect, 0, 0, 0,
                0, f, 0, 0,
                0, 0, (far + near) / (near - far), (2 * far * near) / (near - far),
                0, 0, -1, 0
            );
        }
    }

    public class Camera
    {
        public Vector3 Position;
        public float RotationX;
        public float RotationY;
        public float Distance;

        public Camera()
        {
            Position = new Vector3(0, 0, 0);
            RotationX = 0.5f;
            RotationY = 0.3f;
            Distance = 400f;
        }

        public Vector3 Project(Vector3 point, int width, int height)
        {
            Matrix4 view = Matrix4.Translate(0, 0, -Distance) * 
                          Matrix4.RotateX(-RotationX) * 
                          Matrix4.RotateY(-RotationY);
            
            float fov = 60f * (float)Math.PI / 180f;
            Matrix4 proj = Matrix4.Perspective(fov, (float)width / height, 1f, 2000f);
            Matrix4 mvp = proj * view;

            Vector4 p = new Vector4(point.X, point.Y, point.Z, 1f);
            Vector4 transformed = mvp * p;
            
            if (transformed.W != 0)
            {
                transformed.X /= transformed.W;
                transformed.Y /= transformed.W;
                transformed.Z /= transformed.W;
            }
            
            float screenX = (transformed.X + 1) * 0.5f * width;
            float screenY = (1 - transformed.Y) * 0.5f * height;
            
            return new Vector3(screenX, screenY, transformed.Z);
        }
    }

    public struct Vector4
    {
        public float X, Y, Z, W;

        public Vector4(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }

        public static Vector4 operator *(Matrix4 m, Vector4 v)
        {
            return new Vector4(
                m.M[0, 0] * v.X + m.M[0, 1] * v.Y + m.M[0, 2] * v.Z + m.M[0, 3] * v.W,
                m.M[1, 0] * v.X + m.M[1, 1] * v.Y + m.M[1, 2] * v.Z + m.M[1, 3] * v.W,
                m.M[2, 0] * v.X + m.M[2, 1] * v.Y + m.M[2, 2] * v.Z + m.M[2, 3] * v.W,
                m.M[3, 0] * v.X + m.M[3, 1] * v.Y + m.M[3, 2] * v.Z + m.M[3, 3] * v.W
            );
        }
    }
}