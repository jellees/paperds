#pragma once

class Quaternion
{
private:
	void Init(fx32 ax, fx32 ay, fx32 az, fx32 angle)
	{
		fx32 s = FX_SinIdx(FX_DEG_TO_IDX((angle + 1) >> 1));
		fx32 c = FX_CosIdx(FX_DEG_TO_IDX((angle + 1) >> 1));
		x = FX_Mul(ax, s);
		y = FX_Mul(ay, s);
		z = FX_Mul(az, s);
		w = c;
	}

public:
	Quaternion() { }

	Quaternion(VecFx32* axis, fx32 angle)
	{
		Init(axis->x, axis->y, axis->z, angle);
	}

	Quaternion(fx32 ax, fx32 ay, fx32 az, fx32 angle)
	{
		Init(ax, ay, az, angle);
	}

	Quaternion(VecFx32* forward, VecFx32* up)
	{
		VecFx32 vector;
		VEC_Normalize(forward, &vector);
		VecFx32 vector2;
		VEC_CrossProduct(up, &vector, &vector2);
		VEC_Normalize(&vector2, &vector2);
		VecFx32 vector3;
		VEC_CrossProduct(&vector, &vector2, &vector3);
		fx32 m00 = vector2.x;
		fx32 m01 = vector2.y;
		fx32 m02 = vector2.z;
		fx32 m10 = vector3.x;
		fx32 m11 = vector3.y;
		fx32 m12 = vector3.z;
		fx32 m20 = vector.x;
		fx32 m21 = vector.y;
		fx32 m22 = vector.z;


		fx32 num8 = (m00 + m11) + m22;
		//var quaternion = new Quaternion();
		if (num8 > 0)
		{
			fx32 num = FX_Sqrt(num8 + FX32_ONE);//(float)Math.Sqrt(num8 + 1f);
			w = FX_Mul(num, FX32_HALF);
			num = FX_Div(FX32_HALF, num);//0.5f / num;
			x = FX_Mul(m12 - m21, num);
			y = FX_Mul(m20 - m02, num);
			z = FX_Mul(m01 - m10, num);
			return;
		}
		if ((m00 >= m11) && (m00 >= m22))
		{
			fx32 num7 = FX_Sqrt(((FX32_ONE + m00) - m11) - m22);//(float)Math.Sqrt(((1f + m00) - m11) - m22);
			fx32 num4 = FX_Div(FX32_HALF, num7);
			x = FX_Mul(num7, FX32_HALF);
			y = FX_Mul(m01 + m10, num4);
			z = FX_Mul(m02 + m20, num4);
			w = FX_Mul(m12 - m21, num4);
			return;
		}
		if (m11 > m22)
		{
			fx32 num6 = FX_Sqrt(((FX32_ONE + m11) - m00) - m22);//(float)Math.Sqrt(((1f + m11) - m00) - m22);
			fx32 num3 = FX_Div(FX32_HALF, num6);
			x = FX_Mul(m10 + m01, num3);
			y = FX_Mul(FX32_HALF, num6);
			z = FX_Mul(m21 + m12, num3);
			w = FX_Mul(m20 - m02, num3);
			return; 
		}
		fx32 num5 = FX_Sqrt(((FX32_ONE + m22) - m00) - m11);//(float)Math.Sqrt(((1f + m22) - m00) - m11);
		fx32 num2 = FX_Div(FX32_HALF, num5);
		x = FX_Mul(m20 + m02, num2);
		y = FX_Mul(m21 + m12, num2);
		z = FX_Mul(num5, FX32_HALF);
		w = FX_Mul(m01 - m10, num2);
	}

	/*Quaternion(fx32 rx, fx32 ry, fx32 rz)
	{
        fx32 c1 = FX_CosIdx(FX_DEG_TO_IDX((rx + 1) >> 1));
        fx32 c2 = FX_CosIdx(FX_DEG_TO_IDX((ry + 1) >> 1));
        fx32 c3 = FX_CosIdx(FX_DEG_TO_IDX((rz + 1) >> 1));
        fx32 s1 = FX_SinIdx(FX_DEG_TO_IDX((rx + 1) >> 1));
        fx32 s2 = FX_SinIdx(FX_DEG_TO_IDX((ry + 1) >> 1));
        fx32 s3 = FX_SinIdx(FX_DEG_TO_IDX((rz + 1) >> 1));

        w = FX_Mul(FX_Mul(c1, c2), c3) - FX_Mul(FX_Mul(s1, s2), s3);
        x = FX_Mul(FX_Mul(s1, s2), c3) + FX_Mul(FX_Mul(c1, c2), s3);
        y = FX_Mul(FX_Mul(s1, c2), c3) + FX_Mul(FX_Mul(c1, s2), s3);
        z = FX_Mul(FX_Mul(c1, s2), c3) - FX_Mul(FX_Mul(s1, c2), s3);
	}*/

	fx32 x, y, z, w;

	//mult by vector:
	//aq - br - cs + i(bq + ar - ct) + j(cq + as + bt) + k(at - cr - bs)
	void MultiplyByVector(const VecFx32* vec, VecFx32* dst)
	{
		VecFx32 xyz = {x, y, z};
		VecFx32 t;
		VEC_CrossProduct(&xyz, vec, &t);
		t.x *= 2;
		t.y *= 2;
		t.z *= 2;
		VecFx32 tmp_dst = *vec;
		VEC_MultAdd(w, &t, &tmp_dst, &tmp_dst);
		VecFx32 cross;
		VEC_CrossProduct(&xyz, &t, &cross);
		VEC_Add(&tmp_dst, &cross, &tmp_dst);
		*dst = tmp_dst;
		/*fx32 xx = FX_Mul(vec->x, x) - FX_Mul(vec->y, y) - FX_Mul(vec->z, z);
		fx32 yy = FX_Mul(vec->x, y) + FX_Mul(vec->y, x) - FX_Mul(vec->z, w);
		fx32 zz = FX_Mul(vec->x, z) + FX_Mul(vec->y, w) + FX_Mul(vec->z, x);
		dst->x = xx;
		dst->y = yy;
		dst->z = zz;*/
	}

	void MultiplyByQuaternion(const Quaternion* quaternion, Quaternion* dst)
	{
		VecFx32 xyz;
		xyz.x = FX_Mul(quaternion->w, x);
		xyz.y = FX_Mul(quaternion->w, y);
		xyz.z = FX_Mul(quaternion->w, z);
		xyz.x += FX_Mul(w, quaternion->x);
		xyz.y += FX_Mul(w, quaternion->y);
		xyz.z += FX_Mul(w, quaternion->z);
		VecFx32 xyzl = {x,y,z};
		VecFx32 xyzr = {quaternion->x,quaternion->y,quaternion->z};
		VecFx32 cross;
		VEC_CrossProduct(&xyzl, &xyzr, &cross);
		xyz.x += cross.x;
		xyz.y += cross.y;
		xyz.z += cross.z;
		fx32 w_new = FX_Mul(w, quaternion->w) - VEC_DotProduct(&xyzl, &xyzr);
		dst->x = xyz.x;
		dst->y = xyz.y;
		dst->z = xyz.z;
		dst->w = w_new;
	}

	void Identity()
	{
		x = y = z = 0;
		w = FX32_ONE;
	}

	void Conjugate(Quaternion* dst)
	{
		dst->x = -x;
		dst->y = -y;
		dst->z = -z;
		dst->w = w;
	}

	void Slerp(Quaternion* end, Quaternion* dst, fx32 t)
	{
		/*fx32 cosHalfTheta = FX_Mul(w, end->w) + FX_Mul(x, qb->x) + FX_Mul(y, qb->y) + FX_Mul(z, qb->z);
		if (MATH_ABS(cosHalfTheta) >= FX32_ONE)
		{
			dst->w = w;
			dst->x = x;
			dst->y = y;
			dst->z = z;
			return;
		}
		u16 halfTheta = FX_AcosIdx(cosHalfTheta);
		double sinHalfTheta = sqrt(1.0 - cosHalfTheta*cosHalfTheta);*/
		           // if either input is zero, return the other.
            /*if (q1.LengthSquared == 0.0f)
            {
                if (q2.LengthSquared == 0.0f)
                {
                    return Identity;
                }
                return q2;
            }
            else if (q2.LengthSquared == 0.0f)
            {
                return q1;
            }*/

			fx32 cosHalfAngle = FX_Mul(w, end->w) + FX_Mul(x, end->x) + FX_Mul(y, end->y) + FX_Mul(z, end->z);

			bool negative = FALSE;

            if (MATH_ABS(cosHalfAngle) >= FX32_ONE)
			{
				dst->w = w;
				dst->x = x;
				dst->y = y;
				dst->z = z;
				return;
			}
            else if (cosHalfAngle < 0)
            {
                //q2.Xyz = -q2.Xyz;
                //q2.W = -q2.W;
                cosHalfAngle = -cosHalfAngle;
				negative = TRUE;
            }

            fx32 blendA;
            fx32 blendB;
            if (cosHalfAngle < 4055)//0.99f)
            {
                // do proper slerp for big angles
                u16 halfAngle = FX_AcosIdx(cosHalfAngle);
                fx32 sinHalfAngle = FX_SinIdx(halfAngle);
                fx32 oneOverSinHalfAngle = FX_Div(FX32_ONE, sinHalfAngle);
                blendA = FX_Mul(FX_SinIdx(FX_Mul(halfAngle, FX32_ONE - t)), oneOverSinHalfAngle);
                blendB = FX_Mul(FX_SinIdx(FX_Mul(halfAngle, t)), oneOverSinHalfAngle);
            }
            else
            {
                // do lerp if angle is really small.
                blendA = FX32_ONE - t;
                blendB = t;
            }

			if(!negative)
			{
				dst->x = FX_Mul(x, blendA) + FX_Mul(end->x, blendB);
				dst->y = FX_Mul(y, blendA) + FX_Mul(end->y, blendB);
				dst->z = FX_Mul(z, blendA) + FX_Mul(end->z, blendB);
				dst->w = FX_Mul(w, blendA) + FX_Mul(end->w, blendB);
			}
			else
			{
				dst->x = FX_Mul(x, blendA) + FX_Mul(-end->x, blendB);
				dst->y = FX_Mul(y, blendA) + FX_Mul(-end->y, blendB);
				dst->z = FX_Mul(z, blendA) + FX_Mul(-end->z, blendB);
				dst->w = FX_Mul(w, blendA) + FX_Mul(-end->w, blendB);
			}

           // Quaternion result = new Quaternion(blendA * q1.Xyz + blendB * q2.Xyz, blendA * q1.W + blendB * q2.W);
            //if (result.LengthSquared > 0.0f)
            //    return Normalize(result);
           // else
           //     return Identity;
	}
};