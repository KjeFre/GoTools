/*
 * Copyright (C) 1998, 2000-2007, 2010, 2011, 2012, 2013 SINTEF ICT,
 * Applied Mathematics, Norway.
 *
 * Contact information: E-mail: tor.dokken@sintef.no                      
 * SINTEF ICT, Department of Applied Mathematics,                         
 * P.O. Box 124 Blindern,                                                 
 * 0314 Oslo, Norway.                                                     
 *
 * This file is part of GoTools.
 *
 * GoTools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version. 
 *
 * GoTools is distributed in the hope that it will be useful,        
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with GoTools. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * In accordance with Section 7(b) of the GNU Affero General Public
 * License, a covered work must retain the producer line in every data
 * file that is created or manipulated using GoTools.
 *
 * Other Usage
 * You can be released from the requirements of the license by purchasing
 * a commercial license. Buying such a license is mandatory as soon as you
 * develop commercial activities involving the GoTools library without
 * disclosing the source code of your own applications.
 *
 * This file may be used in accordance with the terms contained in a
 * written agreement between you and SINTEF ICT. 
 */



//#include <stdlib.h>
//#include <time.h>
//#include <math.h>
//#include <iostream>
//#include <fstream>
//#include "GoTools/utils/RegistrationUtils.h"
//#include "GoTools/geometry/ObjectHeader.h"
#include "GoTools/utils/ClosestPointUtils.h"
#include "GoTools/geometry/Cylinder.h"



using namespace std;
using namespace Go;


int main()
{
  // Tests evaluation of closest point function with first and second order derivatives for surface and curve.
  // We use a cylinder since it has a closest point function that can be expressed explicitly.

  // Build cylinder
  Point unit_x(4.0, 1.0, -2.0);
  Point unit_y(1.0, -3.0, 7.0);
  Point location(1.0, 2.0, 3.0);
  double radius = 1.4;

  unit_x.normalize();
  unit_y -= unit_x * (unit_x * unit_y);
  unit_y.normalize();
  Point unit_z = unit_x % unit_y;

  Cylinder* cylinder = new Cylinder(radius, location, unit_z, unit_x);

  // Height range set to [0,1] so one of the points tested has closest point on edge
  double min_v = 0.0;
  double max_v = 1.0;
  cylinder->setParamBoundsV(min_v, max_v);
  cylinder->setParameterDomain(0.0, 2.0 * M_PI, min_v, max_v);

  // Surface model
  vector<shared_ptr<GeomObject> > surfaces;
  surfaces.push_back(shared_ptr<GeomObject>(cylinder));
  shared_ptr<boxStructuring::BoundingBoxStructure> structure = preProcessClosestVectors(surfaces, 1.0);

  for (int clp_type = 0; clp_type < 2; ++clp_type)
  {
    // First iteration: Point on surface. Second iteration: Point on edge curve
    bool on_curve = clp_type == 1;
    Point pt = on_curve ? Point(-1.0, 1.0, 7.0) : Point(3.0, 0.0, 7.0);

    Point pt_min_loc = pt - location;
    double px = pt_min_loc * unit_x;
    double py = pt_min_loc * unit_y;
    double pz = pt_min_loc * unit_z;
    if (!on_curve && (pz <= 0.0 || pz >= 1.0))
    {
      std::cerr << "Closest point is not on surface interior" << std::endl;
      exit(1);
    }
    if (on_curve && pz >= 0.0)
    {
      std::cerr << "Closest point is not on first boundary curve" << std::endl;
      exit(1);
    }

    double px2 = px * px;
    double py2 = py * py;
    double dist2 = px2 + py2;
    double r_dist = radius / sqrt(dist2);
    double r_dist3 = r_dist / dist2;
    double r_dist5 = r_dist3 / dist2;

    // Get partial order derivatives of closest point from explicit closest point function
    vector<Point> expected_derivs;
    Point exp_pt = r_dist * (px * unit_x + py * unit_y) + location;
    if (!on_curve)
      exp_pt += pz * unit_z;
    expected_derivs.push_back(exp_pt);

    // Expected first order derivatives
    Point exp_x = r_dist3 * (py2 * unit_x - py * px * unit_y);
    Point exp_y = r_dist3 * (px2 * unit_y - py * px * unit_x);
    for (int i = 0; i < 3; ++i)
    {
      Point exp_d = exp_x * unit_x[i] + exp_y * unit_y[i];
      if (!on_curve)
	exp_d += unit_z[i] * unit_z;
      expected_derivs.push_back(exp_d);
    }

    // Expected second order derivatives
    Point exp_xx = r_dist5 * (py * (2.0 * px2 - py2) * unit_y - 3.0 * px * py2 * unit_x);
    Point exp_xy = r_dist5 * (py * (2.0 * px2 - py2) * unit_x + px * (2.0 * py2 - px2) * unit_y);
    Point exp_yy = r_dist5 * (px * (2.0 * py2 - px2) * unit_x - 3.0 * py * px2 * unit_y);
    for (int i = 0; i < 3; ++i)
    {
      for (int j = i; j < 3; ++j)
      {
	Point exp_dd = exp_xx * unit_x[i] * unit_x[j] + exp_yy * unit_y[i] * unit_y[j] + exp_xy * (unit_x[i] * unit_y[j] + unit_y[i] * unit_x[j]);
	expected_derivs.push_back(exp_dd);
      }
    }

    // Calculate closest point derivatives by calling closestPointDerivatives()
    vector<float> points_f(3);
    for (int i = 0; i < 3; ++i)
      points_f[i] = float(pt[i]);
    matrix3DUtils::matrix3D id = matrix3DUtils::identity3D();
    Point tr(0.0, 0.0, 0.0);
    vector<float> derivs_f = closestPointDerivatives(points_f, structure, id, tr);

    // Compare with expected values. Tolerance must be lower for edge curve, probably due to uncertainty with the rational spline function
    double tol = on_curve ? 0.005 : 1e-7;
    for (int i = 0, idx = 0; i < 10; ++i, idx += 3)
    {
      Point calculated_pt(derivs_f[idx], derivs_f[idx + 1], derivs_f[idx + 2]);
      double exp_dist = calculated_pt.dist(expected_derivs[i]);
      if (exp_dist > tol)
      {
	std::cerr << "Closest point derivative " << i << " on " << (on_curve ? "edge" : "surface interior") << " did not match with expected" << std::endl;
	exit(1);
      }
    }
  }

  std::cout << "All tests passed" << std::endl;
}
