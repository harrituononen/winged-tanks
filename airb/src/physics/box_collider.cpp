#include <iostream>
#include "box_collider.hpp"
#include "collision.hpp"
#include "../math/general.hpp"
#include "rigid_body_with_collider.hpp"

namespace physics
{
    BoxCollider::BoxCollider(std::vector<math::Vector2f> const& vertexes)
        : m_vertexes(vertexes)
        , m_prev_vertexes()
        , m_axes()
        , m_center((vertexes[0] + vertexes[2]) * 0.5f)
        , m_size()
    {
        for (int i = 0; i < 4; i++)
            m_prev_vertexes.push_back(m_vertexes[i]);
        if (vertexes.size() == 4)
        {
            m_axes.push_back((m_vertexes[1] - m_vertexes[0]).normalized());
            m_axes.push_back((m_vertexes[1] - m_vertexes[2]).normalized());
        }
        else
        {
            m_axes.push_back(math::Vector2f({ -(m_vertexes[0][Y] - m_vertexes[2][Y]),
                                            (m_vertexes[0][X] - m_vertexes[2][X]) }).normalized());
            m_axes.push_back(math::Vector2f({ -(m_vertexes[1][Y] - m_vertexes[0][Y]),
                                            (m_vertexes[1][X] - m_vertexes[0][X]) }).normalized());
            m_axes.push_back(math::Vector2f({ -(m_vertexes[2][Y] - m_vertexes[1][Y]),
                                            (m_vertexes[2][X] - m_vertexes[1][X]) }).normalized());
        }
        if (vertexes.size() == 4)
        {
            m_size = (int)((m_vertexes[1] - m_vertexes[0]).magnitude() *
                (m_vertexes[1] - m_vertexes[2]).magnitude());
        }
        else
        {
            m_size = (int)ceil(0.5 * abs(vertexes[0][X] * vertexes[1][Y] - vertexes[0][X] * vertexes[2][Y] +
                                    vertexes[1][X] * vertexes[2][Y] - vertexes[1][X] * vertexes[0][Y] +
                                    vertexes[2][X] * vertexes[0][Y] - vertexes[2][X] * vertexes[1][Y]));
        }
    }

    BoxCollider::BoxCollider(BoxCollider const& bc)
        : m_vertexes(bc.m_vertexes)
        , m_prev_vertexes(bc.m_prev_vertexes)
        , m_axes(bc.m_axes)
        , m_center(bc.m_center)
        , m_size(bc.m_size)
        , m_rb(bc.m_rb)
    {}

    BoxCollider& BoxCollider::operator=(BoxCollider const& rhs)
    {
        m_vertexes = rhs.m_vertexes;
        m_prev_vertexes = rhs.m_prev_vertexes;
        m_axes = rhs.m_axes;
        m_center = rhs.m_center;
        m_size = rhs.m_size;
        m_rb = rhs.m_rb;
        return *this;
    }
    void BoxCollider::update_position(math::Vector2f const& change)
    {
        for (int i = 0; i < 4; i++)
        {
            m_prev_vertexes[i] = m_vertexes[i];
            m_vertexes[i] += change;
        }

        m_center = (m_vertexes[0] + m_vertexes[2]) * 0.5f;
        m_axes[0] = (m_vertexes[1] - m_vertexes[0]).normalized();
        m_axes[1] = (m_vertexes[1] - m_vertexes[2]).normalized();
    }

    void BoxCollider::set_position(math::Vector2f const& pos, math::Vector2f const& offset_tl,
                      math::Vector2f const& offset_tr, math::Vector2f const& offset_br,
                      math::Vector2f const& offset_bl)
    {
        m_vertexes[0] = pos+offset_tl;
        m_vertexes[1] = pos+offset_tr;
        m_vertexes[2] = pos+offset_br;
        m_vertexes[3] = pos+offset_bl;
        for (int i = 0; i < 4; i++)
        {
            m_prev_vertexes[i] = m_vertexes[i];
        }
        m_center = (m_vertexes[0] + m_vertexes[2]) * 0.5f;
        m_axes[0] = (m_vertexes[1] - m_vertexes[0]).normalized();
        m_axes[1] = (m_vertexes[1] - m_vertexes[2]).normalized();
    }
    void BoxCollider::update_rotation(float rotation, math::Vector2f point)
    {
        for (int i = 0; i < 4; i++)
        {
            float s = sinf(rotation);
            float c = cosf(rotation);
            float xold = m_vertexes[i][X] - point[X];
            float yold = m_vertexes[i][Y] - point[Y];
            float xnew = xold * c - yold * s;
            float ynew = xold * s + yold * c;
            m_vertexes[i] = math::Vector2f({ xnew + point[X], ynew + point[Y] });
        }
        m_center = (m_vertexes[0] + m_vertexes[2]) * 0.5f;
        m_axes[0] = (m_vertexes[1] - m_vertexes[0]).normalized();
        m_axes[1] = (m_vertexes[1] - m_vertexes[2]).normalized();
    }

    void BoxCollider::revert()
    {
        for (int i = 0; i < 4; i++)
            m_vertexes[i] = m_prev_vertexes[i];
    }

    Collision BoxCollider::check_collision(BoxCollider& b)
    {
        std::vector<math::Vector2f> axes = { m_axes[0], m_axes[1], b.m_axes[0], b.m_axes[1] };
        for (unsigned i = 0; i < m_axes.size(); i++)
            axes.push_back(m_axes[i]);
        for (unsigned i = 0; i < b.m_axes.size(); i++)
            axes.push_back(b.m_axes[i]);
        int coll = 0;
        float overlap = 10000;
        math::Vector2f smallest;
        float a_min, a_max, b_min, b_max;

        for (unsigned i = 0; i < axes.size(); i++)
        {
            a_min = m_vertexes[0].dot(axes[i]);
            a_max = m_vertexes[0].dot(axes[i]);
            b_min = b.m_vertexes[0].dot(axes[i]);
            b_max = b.m_vertexes[0].dot(axes[i]);
            for (unsigned j = 1; j < m_vertexes.size(); j++)
            {
                float n = m_vertexes[j].dot(axes[i]);
                if (n < a_min)
                    a_min = n;
                if (n > a_max)
                    a_max = n;
            }
            for (unsigned j = 1; j < b.m_vertexes.size(); j++)
            {
                float n = b.m_vertexes[j].dot(axes[i]);
                if (n < b_min)
                    b_min = n;
                if (n > b_max)
                    b_max = n;
            }
            if (b_min > a_max || b_max < a_min)
            {
                return Collision(math::Vector2f::zero(), coll);
            }
            else
            {
                float o;
                if ((a_min <= b_min && a_max >= b_max) || (b_min <= a_min && b_max >= a_max))
                {
                    o = i < 2 ? b_max - b_min : a_max - a_min;
                }
                else
                {
                    o = i < 2 ? b_max - a_min : a_max - b_min;
                }

                if (o < overlap)
                {
                    overlap = o;
                    smallest = axes[i];
                    coll = i < 2 ? 2 : 1;
                }
            }

        }
        return Collision(overlap * smallest, coll);
    }

    float BoxCollider::signum(float n)
    {
        return static_cast<float>((n > 0) - (n < 0));
    }
    TerrainCollision BoxCollider::check_terrain(RigidBodyWithCollider const& rb, entities::LevelTerrain& terr,
                                                std::chrono::milliseconds const dt, std::vector<int> m_alphas)
    {
        float const sec = static_cast<float>(static_cast<double>(dt.count()) * 1e-3);
        math::Vector2f vel = rb.get_velocity() * sec;
        float ang_vel = rb.get_angular_velocity() * sec;
        math::Vector2f position = rb.get_position();
        float rotation = rb.get_rotation();
        BoxCollider collider = *this;
        int iterations = 5;
        TerrainCollision coll(math::Vector2f::zero(), math::Vector2f::zero(), 1.0f);
        std::vector<math::Vector2i> pixels_to_clear;
        math::Vector2f point;
        math::Vector2f normal;
        float correction;
        std::vector<math::Vector2f> vert;
        for (int i = 1; i <= iterations; ++i)
        {
            float prev_rotation = rotation;
            position += vel / (float)iterations;
            collider.update_position(vel / (float)iterations);
            rotation += ang_vel / (float)iterations;

            float const s = rotation < -math::DOUBLE_PI ? 1.0f
                : rotation >  math::DOUBLE_PI ? -1.0f
                : 0.0f;

            rotation += s * math::DOUBLE_PI;
            collider.update_rotation(rotation - prev_rotation, position);
            vert = collider.get_vertexes();
            for (int k = 0; k < 8; k += 2)
            {
                math::Vector2f move = vert[m_alphas[k]] - vert[m_alphas[k + 1]];
                int magn = (int)move.magnitude();
                math::Vector2f norm = move.normalized();
                for (int j = 0; j <= magn; ++j)
                {
                    math::Vector2i vec(vert[m_alphas[k + 1]] + (float)j * norm);
                    point = (vert[m_alphas[k + 1]] + (float)j * norm);
                    correction = ((float)i - 1.0f) / (float)iterations;
                    if (vec[X] < 0)
                    {
                        normal = math::Vector2f({1.0f, 0.0f});
                        return TerrainCollision(point, normal, correction);
                    }
                    else if (vec[X] >= 6000)
                    {
                        normal = math::Vector2f({-1.0f, 0.0f });
                        return TerrainCollision(point, normal, correction);                  
                    }
                    else if (vec[Y] < 0)
                    {
                        normal = math::Vector2f({0.0f, 1.0f });
                        return TerrainCollision(point, normal, correction);                 
                    }                       
                    else if (vec[Y] >= 4000)
                    {
                        normal = math::Vector2f({0.0f, -1.0f });
                        return TerrainCollision(point, normal, correction);
                    }
                    else if (terr.is_pixel_solid(vec))
                    {

                        if (j == 0)
                        {
                            int f = k < 2 ? 6 : -2;
                            math::Vector2f mov = vert[m_alphas[k + f]] - vert[m_alphas[k + 1 + f]];
                            normal = (math::Vector2f({ -move[Y], move[X] }) + math::Vector2f({ -mov[Y], mov[X] })).normalized();
                        }
                        else if (j == magn)
                        {
                            int f = k > 5 ? -6 : 2;
                            math::Vector2f mov = vert[m_alphas[k + f]] - vert[m_alphas[k + 1 + f]];
                            normal = (math::Vector2f({ -move[Y], move[X] }) + math::Vector2f({ -mov[Y], mov[X] })).normalized();
                        }
                        else
                            normal = math::Vector2f({ -move[Y], move[X] }).normalized();
                        return TerrainCollision(point, normal, correction);
                }
            }
            }
        }
        return coll;
    }


} // namespace physics