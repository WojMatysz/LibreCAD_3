#include "lc_maths.h"
#include <cad/math/equation.h>
#include <cad/math/intersect.h>

void import_lc_maths_namespace(sol::state & luaVM) {
    /*
    sol::table lc = luaVM["lc"];
    lc["maths"] = luaVM.create_table();
    sol::table maths = lc["maths"];

    maths.new_usertype<lc::maths::Equation>(
            "Equation",
            sol::constructors<
            lc::maths::Equation(), 
            lc::maths::Equation(double, double, double, double, double, double), 
            lc::maths::Equation(const std::vector<double> &)
            >(),
            "Coefficients", &lc::maths::Equation::Coefficients,
            "Matrix", &lc::maths::Equation::Matrix,
            "flipXY", &lc::maths::Equation::flipXY,
            "move", &lc::maths::Equation::move,
            "rotate", sol::overload(
                static_cast<const lc::maths::Equation(lc::maths::Equation::*)(double) const>(&lc::maths::Equation::rotate), 
                static_cast<const lc::maths::Equation(lc::maths::Equation::*)(const lc::geo::Coordinate &, double) const>(&lc::maths::Equation::rotate)
                ),
            "rotationMatrix", &lc::maths::Equation::rotationMatrix,
            "translateMatrix", &lc::maths::Equation::translateMatrix
            );

    maths.new_usertype<lc::maths::IntersectMany>(
            "IntersectMany",
            sol::constructors<
            lc::maths::IntersectMany(std::vector<lc::entity::CADEntity_CSPtr>, lc::maths::Intersect::Method, double), 
            lc::maths::IntersectMany(std::vector<lc::entity::CADEntity_CSPtr>)
            >(),
            "result", &lc::maths::IntersectMany::result
            );

    maths.new_usertype<lc::maths::IntersectAgainstOthers>(
            "IntersectAgainstOthers",
            sol::constructors<
            lc::maths::IntersectAgainstOthers(std::vector<lc::entity::CADEntity_CSPtr>, std::vector<lc::entity::CADEntity_CSPtr>, lc::maths::Intersect::Method, double)>(),
        "result", &lc::maths::IntersectAgainstOthers::result
            );
            */
}
