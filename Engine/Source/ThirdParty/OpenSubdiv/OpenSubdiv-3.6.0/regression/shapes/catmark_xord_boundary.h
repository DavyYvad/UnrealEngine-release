//
//   Copyright 2019 DreamWorks Animation LLC.
//
//   Licensed under the Apache License, Version 2.0 (the "Apache License")
//   with the following modification; you may not use this file except in
//   compliance with the Apache License and the following modification to it:
//   Section 6. Trademarks. is deleted and replaced with:
//
//   6. Trademarks. This License does not grant permission to use the trade
//      names, trademarks, service marks, or product names of the Licensor
//      and its affiliates, except as required to comply with Section 4(c) of
//      the License and to reproduce the content of the NOTICE file.
//
//   You may obtain a copy of the Apache License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the Apache License with the above modification is
//   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//   KIND, either express or implied. See the Apache License for the specific
//   language governing permissions and limitations under the Apache License.
//

static const std::string catmark_xord_boundary =
"#\n"
"#   Four shapes ordered left->right and top->bottom in the XZ plane\n"
"#\n"
"#   Shape 1:  top-left\n"
"#\n"
"v -0.750000 -0.480000  0.750000\n"
"v -0.230385  0.000000  0.750000\n"
"v -0.230385  0.000000  1.050000\n"
"v -0.490192  0.000000  1.200000\n"
"v -0.750000  0.000000  1.350000\n"
"v -1.009808  0.000000  1.200000\n"
"v -1.269615  0.000000  1.050000\n"
"v -1.269615  0.000000  0.750000\n"
"\n"
"vt  0.250000  0.750000\n"
"vt  0.423205  0.750000\n"
"vt  0.423205  0.850000\n"
"vt  0.336603  0.900000\n"
"vt  0.250000  0.950000\n"
"vt  0.163397  0.900000\n"
"vt  0.076795  0.850000\n"
"vt  0.076795  0.750000\n"
"\n"
"f  1/1   2/2   3/3   4/4\n"
"f  6/6   1/1   4/4   5/5\n"
"f  6/6   7/7   8/8   1/1\n"
"\n"
"#\n"
"#   Shape 2:  top-right\n"
"#\n"
"v  0.750000 -0.480000  0.750000\n"
"v  1.304328  0.000000  0.750000\n"
"v  1.304328  0.000000  0.979610\n"
"v  1.141969  0.000000  1.141969\n"
"v  0.979610  0.000000  1.304328\n"
"v  0.750000  0.000000  1.304328\n"
"v  0.520390  0.000000  1.304328\n"
"v  0.358031  0.000000  1.141969\n"
"v  0.195672  0.000000  0.979610\n"
"v  0.195672  0.000000  0.750000\n"
"\n"
"vt  0.750000  0.750000\n"
"vt  0.934776  0.750000\n"
"vt  0.934776  0.826537\n"
"vt  0.880656  0.880656\n"
"vt  0.826537  0.934776\n"
"vt  0.750000  0.934776\n"
"vt  0.673463  0.934776\n"
"vt  0.619344  0.880656\n"
"vt  0.565224  0.826537\n"
"vt  0.565224  0.750000\n"
"\n"
"f  9/9  10/10 11/11 12/12\n"
"f 14/14  9/9  12/12 13/13\n"
"f 15/15 16/16  9/9  14/14\n"
"f 16/16 17/17 18/18  9/9\n"
"\n"
"#\n"
"#   Shape 3:  bottom-left\n"
"#\n"
"v -0.750000 -0.480000 -0.750000\n"
"v -0.179366  0.000000 -0.750000\n"
"v -0.179366  0.000000 -0.564590\n"
"v -0.288347  0.000000 -0.414590\n"
"v -0.397329  0.000000 -0.264590\n"
"v -0.573664  0.000000 -0.207295\n"
"v -0.750000  0.000000 -0.150000\n"
"v -0.926336  0.000000 -0.207295\n"
"v -1.102671  0.000000 -0.264590\n"
"v -1.211653  0.000000 -0.414590\n"
"v -1.320634  0.000000 -0.564590\n"
"v -1.320634  0.000000 -0.750000\n"
"\n"
"vt  0.250000  0.250000\n"
"vt  0.440211  0.250000\n"
"vt  0.440211  0.311803\n"
"vt  0.403884  0.361803\n"
"vt  0.367557  0.411803\n"
"vt  0.308779  0.430902\n"
"vt  0.250000  0.450000\n"
"vt  0.191221  0.430902\n"
"vt  0.132443  0.411803\n"
"vt  0.096116  0.361803\n"
"vt  0.059789  0.311803\n"
"vt  0.059789  0.250000\n"
"\n"
"f 19/19 20/20 21/21 22/22\n"
"f 24/24 19/19 22/22 23/23\n"
"f 25/25 26/26 19/19 24/24\n"
"f 26/26 27/27 28/28 19/19\n"
"f 19/19 28/28 29/29 30/30\n"
"\n"
"#\n"
"#   Shape 4:  bottom-right\n"
"#\n"
"v  0.750000 -0.480000 -0.750000\n"
"v  1.329556  0.000000 -0.750000\n"
"v  1.329556  0.000000 -0.594709\n"
"v  1.251910  0.000000 -0.460222\n"
"v  1.174264  0.000000 -0.325736\n"
"v  1.039778  0.000000 -0.248090\n"
"v  0.905291  0.000000 -0.170444\n"
"v  0.750000  0.000000 -0.170444\n"
"v  0.594709  0.000000 -0.170444\n"
"v  0.460222  0.000000 -0.248090\n"
"v  0.325736  0.000000 -0.325736\n"
"v  0.248090  0.000000 -0.460222\n"
"v  0.170444  0.000000 -0.594709\n"
"v  0.170444  0.000000 -0.750000\n"
"\n"
"vt  0.750000  0.250000\n"
"vt  0.943185  0.250000\n"
"vt  0.943185  0.301764\n"
"vt  0.917303  0.346593\n"
"vt  0.891421  0.391421\n"
"vt  0.846593  0.417303\n"
"vt  0.801764  0.443185\n"
"vt  0.750000  0.443185\n"
"vt  0.698236  0.443185\n"
"vt  0.653407  0.417303\n"
"vt  0.608579  0.391421\n"
"vt  0.582697  0.346593\n"
"vt  0.556815  0.301764\n"
"vt  0.556815  0.250000\n"
"\n"
"f 31/31 32/32 33/33 34/34\n"
"f 36/36 31/31 34/34 35/35\n"
"f 37/37 38/38 31/31 36/36\n"
"f 38/38 39/39 40/40 31/31\n"
"f 31/31 40/40 41/41 42/42\n"
"f 31/31 42/42 43/43 44/44\n"
"\n"
"t interpolateboundary 1/0/0 1\n"
"\n"
;