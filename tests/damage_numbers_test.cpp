#include "../src/damage_numbers.h"
#include <cassert>
#include <cmath>
#include <cstdio>

int main()
{
    using namespace ormenos;
    assert(is_damage_number_style("Records\\UI\\FontStyles\\DamageNormal.dbr"));
    assert(is_damage_number_style("records/ui/fontstyles/criticalhitonmonster.dbr"));
    assert(!is_damage_number_style("Records\\UI\\FontStyles\\Healing.dbr"));
    assert(!is_damage_number_style("Damage/UI/FontStyles/Normal.dbr"));
    assert(!is_damage_number_style(""));
    for (int lifetime : {1500, 2000})
    {
        auto start = animate_damage_number(lifetime, lifetime, 0);
        auto peak = animate_damage_number(lifetime, lifetime / 2, 0);
        auto end = animate_damage_number(lifetime, 0, 0);
        assert(start.scale == 1.0f && start.alpha == 1.0f && start.x == 0 && start.y == 0);
        assert(std::abs(peak.scale - 1.4f) < 0.001f && peak.alpha == 1.0f);
        assert(end.scale < start.scale && end.alpha == 0);
        float previousX = 0;
        for (int age = 0; age <= lifetime; ++age)
        {
            auto left = animate_damage_number(lifetime, lifetime - age, 0);
            auto right = animate_damage_number(lifetime, lifetime - age, 1);
            assert(left.x <= previousX && left.y <= 0);
            assert(left.x == -right.x && left.y == right.y);
            assert(left.scale >= 0.54f && left.scale <= 1.401f);
            assert(left.alpha >= 0 && left.alpha <= 1);
            if (age * 100 <= lifetime * 65)
                assert(left.scale >= 1.0f && left.alpha == 1.0f);
            previousX = left.x;
        }
        assert(animate_damage_number(lifetime, lifetime + 1, 0).x == 0);
        assert(animate_damage_number(lifetime, -1, 0).alpha == 0);
    }
    assert(animate_damage_number(0, 0, 0).alpha == 0);
    std::puts("Damage number curves, lifetime limits, styles, and outward trajectories passed.");
}
