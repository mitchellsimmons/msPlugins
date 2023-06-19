#include <maya/MFnPlugin.h>

#include "arithmetic/add_node.h"
#include "arithmetic/addAngle_node.h"
#include "arithmetic/addInt_node.h"
#include "arithmetic/average_node.h"
#include "arithmetic/averageAngle_node.h"
#include "arithmetic/averageInt_node.h"
#include "arithmetic/divide_node.h"
#include "arithmetic/divideByInt_node.h"
#include "arithmetic/divideAngle_node.h"
#include "arithmetic/divideAngleByInt_node.h"
#include "arithmetic/divideInt_node.h"
#include "arithmetic/modulus_node.h"
#include "arithmetic/multiply_node.h"
#include "arithmetic/multiplyByInt_node.h"
#include "arithmetic/multiplyAngle_node.h"
#include "arithmetic/multiplyAngleByInt_node.h"
#include "arithmetic/multiplyInt_node.h"
#include "arithmetic/negate_node.h"
#include "arithmetic/negateAngle_node.h"
#include "arithmetic/negateInt_node.h"
#include "arithmetic/normalizeDoubleArray_node.h"
#include "arithmetic/normalizeWeightArray_node.h"
#include "arithmetic/reciprocal_node.h"
#include "arithmetic/subtract_node.h"
#include "arithmetic/subtractAngle_node.h"
#include "arithmetic/subtractInt_node.h"
#include "arithmetic/sum_node.h"
#include "arithmetic/sumAngle_node.h"
#include "arithmetic/sumInt_node.h"
#include "arithmetic/weightedAverage_node.h"
#include "arithmetic/weightedAverageAngle_node.h"
#include "arithmetic/weightedAverageInt_node.h"
#include "condition/condition_node.h"
#include "condition/conditionAngle_node.h"
#include "condition/conditionInt_node.h"
#include "condition/max_node.h"
#include "condition/maxAngle_node.h"
#include "condition/maxInt_node.h"
#include "condition/maxElement_node.h"
#include "condition/maxElementAngle_node.h"
#include "condition/maxElementInt_node.h"
#include "condition/min_node.h"
#include "condition/minAngle_node.h"
#include "condition/minInt_node.h"
#include "condition/minElement_node.h"
#include "condition/minElementAngle_node.h"
#include "condition/minElementInt_node.h"
#include "condition/select_node.h"
#include "condition/selectDoubleArray_node.h"
#include "condition/selectAngle_node.h"
#include "condition/selectAngleArray_node.h"
#include "condition/selectCurve_node.h"
#include "condition/selectEuler_node.h"
#include "condition/selectEulerArray_node.h"
#include "condition/selectInt_node.h"
#include "condition/selectIntArray_node.h"
#include "condition/selectMatrix_node.h"
#include "condition/selectMatrixArray_node.h"
#include "condition/selectMesh_node.h"
#include "condition/selectQuaternion_node.h"
#include "condition/selectQuaternionArray_node.h"
#include "condition/selectSurface_node.h"
#include "condition/selectVector_node.h"
#include "condition/selectVectorArray_node.h"
#include "distance/absolute_node.h"
#include "distance/absoluteAngle_node.h"
#include "distance/absoluteInt_node.h"
#include "distance/distanceBetweenFrames_node.h"
#include "distance/distanceBetweenPoints_node.h"
#include "euler/averageEuler_node.h"
#include "euler/convertEulerToMatrix_node.h"
#include "euler/convertEulerToQuaternion_node.h"
#include "euler/divideEuler_node.h"
#include "euler/multiplyEuler_node.h"
#include "euler/weightedAverageEuler_node.h"
#include "interpolate/lerp_node.h"
#include "interpolate/lerpAngle_node.h"
#include "interpolate/lerpMatrix_node.h"
#include "interpolate/lerpVector_node.h"
#include "interpolate/slerpQuaternion_node.h"
#include "interpolate/smoothstep_node.h"
#include "interpolate/smoothstepAngle_node.h"
#include "interpolate/smoothstepMatrix_node.h"
#include "interpolate/smoothstepVector_node.h"
#include "logic/andBool_node.h"
#include "logic/andInt_node.h"
#include "logic/notBool_node.h"
#include "logic/notInt_node.h"
#include "logic/orBool_node.h"
#include "logic/orInt_node.h"
#include "logic/xorBool_node.h"
#include "logic/xorInt_node.h"
#include "matrix/averageMatrix_node.h"
#include "matrix/composeMatrix_node.h"
#include "matrix/decomposeMatrix_node.h"
#include "matrix/extractBasisXFromMatrix_node.h"
#include "matrix/extractBasisYFromMatrix_node.h"
#include "matrix/extractBasisZFromMatrix_node.h"
#include "matrix/extractEulerFromMatrix_node.h"
#include "matrix/extractQuaternionFromMatrix_node.h"
#include "matrix/extractRotationMatrix_node.h"
#include "matrix/extractScaleFromMatrix_node.h"
#include "matrix/extractScaleMatrix_node.h"
#include "matrix/extractTranslationFromMatrix_node.h"
#include "matrix/extractTranslationMatrix_node.h"
#include "matrix/multiplyMatrix_node.h"
#include "matrix/multiplyMatrixArray_node.h"
#include "matrix/postMultiplyMatrixByAxisXAngle_node.h"
#include "matrix/postMultiplyMatrixByAxisYAngle_node.h"
#include "matrix/postMultiplyMatrixByAxisZAngle_node.h"
#include "matrix/postMultiplyMatrixByEuler_node.h"
#include "matrix/postMultiplyMatrixByQuaternion_node.h"
#include "matrix/postMultiplyMatrixByScale_node.h"
#include "matrix/postMultiplyMatrixByTranslation_node.h"
#include "matrix/preMultiplyMatrixByAxisXAngle_node.h"
#include "matrix/preMultiplyMatrixByAxisYAngle_node.h"
#include "matrix/preMultiplyMatrixByAxisZAngle_node.h"
#include "matrix/preMultiplyMatrixByEuler_node.h"
#include "matrix/preMultiplyMatrixByQuaternion_node.h"
#include "matrix/preMultiplyMatrixByScale_node.h"
#include "matrix/preMultiplyMatrixByTranslation_node.h"
#include "matrix/weightedAverageMatrix_node.h"
#include "power/power_node.h"
#include "power/squareRoot_node.h"
#include "quaternion/averageQuaternion_node.h"
#include "quaternion/convertQuaternionToEuler_node.h"
#include "quaternion/convertQuaternionToMatrix_node.h"
#include "quaternion/multiplyQuaternion_node.h"
#include "quaternion/multiplyQuaternionArray_node.h"
#include "quaternion/weightedAverageQuaternion_node.h"
#include "range/clamp_node.h"
#include "range/clampAngle_node.h"
#include "range/clampInt_node.h"
#include "range/remap_node.h"
#include "range/remapAngle_node.h"
#include "range/remapInt_node.h"
#include "round/ceil_node.h"
#include "round/ceilAngle_node.h"
#include "round/floor_node.h"
#include "round/floorAngle_node.h"
#include "round/round_node.h"
#include "round/roundAngle_node.h"
#include "trigonometry/acos_node.h"
#include "trigonometry/asin_node.h"
#include "trigonometry/atan_node.h"
#include "trigonometry/atan2_node.h"
#include "trigonometry/cartesianToPolar_node.h"
#include "trigonometry/cos_node.h"
#include "trigonometry/polarToCartesian_node.h"
#include "trigonometry/sin_node.h"
#include "trigonometry/tan_node.h"
#include "vector/addVector_node.h"
#include "vector/angleBetweenVectors_node.h"
#include "vector/averageVector_node.h"
#include "vector/closestPointOnLine_node.h"
#include "vector/closestPointOnPlane_node.h"
#include "vector/crossProduct_node.h"
#include "vector/divideVector_node.h"
#include "vector/dotProduct_node.h"
#include "vector/limitVector_node.h"
#include "vector/multiplyPointByMatrix_node.h"
#include "vector/multiplyVector_node.h"
#include "vector/multiplyVectorByMatrix_node.h"
#include "vector/negateVector_node.h"
#include "vector/normalizeVector_node.h"
#include "vector/reflectVector_node.h"
#include "vector/rotateVectorByAxisAngle_node.h"
#include "vector/rotateVectorByEuler_node.h"
#include "vector/rotateVectorByQuaternion_node.h"
#include "vector/signedAngleBetweenVectors_node.h"
#include "vector/subtractVector_node.h"
#include "vector/sumVector_node.h"
#include "vector/vectorLength_node.h"
#include "vector/vectorLengthSquared_node.h"
#include "vector/vectorPlaneProjection_node.h"
#include "vector/vectorProjection_node.h"
#include "vector/weightedAverageVector_node.h"

#include "utils/macros.h"
#include "utils/plugin_utils.h"

// ------ kTypeId ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// |---------|
// |  1.0.0  |
// |---------|

// ------ ARITHMETIC ------
// Project Block0 IDs : [0x00131010 - 0x0013101f]
const MTypeId Add::kTypeId = 0x00131010;
const MTypeId AddAngle::kTypeId = 0x00131011;
const MTypeId AddInt::kTypeId = 0x00131012;
const MTypeId Average::kTypeId = 0x00131013;
const MTypeId AverageAngle::kTypeId = 0x00131014;
const MTypeId AverageInt::kTypeId = 0x00131015;
const MTypeId Divide::kTypeId = 0x00131016;
const MTypeId DivideByInt::kTypeId = 0x00131017;
const MTypeId DivideAngle::kTypeId = 0x00131018;
const MTypeId DivideAngleByInt::kTypeId = 0x00131019;
const MTypeId DivideInt::kTypeId = 0x0013101a;
const MTypeId Modulus::kTypeId = 0x0013101b;
const MTypeId Multiply::kTypeId = 0x0013101c;
const MTypeId MultiplyByInt::kTypeId = 0x0013101d;
const MTypeId MultiplyAngle::kTypeId = 0x0013101e;
const MTypeId MultiplyAngleByInt::kTypeId = 0x0013101f;
// Project Block1 IDs : [0x00131040 - 0x0013104f]
const MTypeId MultiplyInt::kTypeId = 0x00131040;
const MTypeId Negate::kTypeId = 0x00131041;
const MTypeId NegateAngle::kTypeId = 0x00131042;
const MTypeId NegateInt::kTypeId = 0x00131043;
const MTypeId NormalizeDoubleArray::kTypeId = 0x00131044;
const MTypeId NormalizeWeightArray::kTypeId = 0x00131045;
const MTypeId Reciprocal::kTypeId = 0x00131046;
const MTypeId Subtract::kTypeId = 0x00131047;
const MTypeId SubtractAngle::kTypeId = 0x00131048;
const MTypeId SubtractInt::kTypeId = 0x00131049;
const MTypeId Sum::kTypeId = 0x0013104a;
const MTypeId SumAngle::kTypeId = 0x0013104b;
const MTypeId SumInt::kTypeId = 0x0013104c;
const MTypeId WeightedAverage::kTypeId = 0x0013104d;
const MTypeId WeightedAverageAngle::kTypeId = 0x0013104e;
const MTypeId WeightedAverageInt::kTypeId = 0x0013104f;

// ------ CONDITION ------
// Project Block2 IDs : [0x00131050 - 0x0013105f]
const MTypeId Condition::kTypeId = 0x00131050;
const MTypeId ConditionAngle::kTypeId = 0x00131051;
const MTypeId ConditionInt::kTypeId = 0x00131052;
const MTypeId Max::kTypeId = 0x00131053;
const MTypeId MaxAngle::kTypeId = 0x00131054;
const MTypeId MaxInt::kTypeId = 0x00131055;
const MTypeId MaxElement::kTypeId = 0x00131056;
const MTypeId MaxElementAngle::kTypeId = 0x00131057;
const MTypeId MaxElementInt::kTypeId = 0x00131058;
const MTypeId Min::kTypeId = 0x00131059;
const MTypeId MinAngle::kTypeId = 0x0013105a;
const MTypeId MinInt::kTypeId = 0x0013105b;
const MTypeId MinElement::kTypeId = 0x0013105c;
const MTypeId MinElementAngle::kTypeId = 0x0013105d;
const MTypeId MinElementInt::kTypeId = 0x0013105e;
const MTypeId Select::kTypeId = 0x0013105f;
// Project Block3 IDs : [0x00131060 - 0x0013106f]
const MTypeId SelectDoubleArray::kTypeId = 0x00131060;
const MTypeId SelectAngle::kTypeId = 0x00131061;
const MTypeId SelectAngleArray::kTypeId = 0x00131062;
const MTypeId SelectCurve::kTypeId = 0x00131063;
const MTypeId SelectEuler::kTypeId = 0x00131064;
const MTypeId SelectEulerArray::kTypeId = 0x00131065;
const MTypeId SelectInt::kTypeId = 0x00131066;
const MTypeId SelectIntArray::kTypeId = 0x00131067;
const MTypeId SelectMatrix::kTypeId = 0x00131068;
const MTypeId SelectMatrixArray::kTypeId = 0x00131069;
const MTypeId SelectMesh::kTypeId = 0x0013106a;
const MTypeId SelectQuaternion::kTypeId = 0x0013106b;
const MTypeId SelectQuaternionArray::kTypeId = 0x0013106c;
const MTypeId SelectSurface::kTypeId = 0x0013106d;
const MTypeId SelectVector::kTypeId = 0x0013106e;
const MTypeId SelectVectorArray::kTypeId = 0x0013106f;

// ------ DISTANCE ------
// Project Block4 IDs : [0x00131070 - 0x0013107f]
const MTypeId Absolute::kTypeId = 0x00131070;
const MTypeId AbsoluteAngle::kTypeId = 0x00131071;
const MTypeId AbsoluteInt::kTypeId = 0x00131072;
const MTypeId DistanceBetweenFrames::kTypeId = 0x00131073;
const MTypeId DistanceBetweenPoints::kTypeId = 0x00131074;

// ------ EULER ------
const MTypeId AverageEuler::kTypeId = 0x00131075;
const MTypeId ConvertEulerToMatrix::kTypeId = 0x00131076;
const MTypeId ConvertEulerToQuaternion::kTypeId = 0x00131077;
const MTypeId DivideEuler::kTypeId = 0x00131078;
const MTypeId MultiplyEuler::kTypeId = 0x00131079;
const MTypeId WeightedAverageEuler::kTypeId = 0x0013107a;

// ------ INTERPOLATION ------
const MTypeId Lerp::kTypeId = 0x0013107b;
const MTypeId LerpAngle::kTypeId = 0x0013107c;
const MTypeId LerpMatrix::kTypeId = 0x0013107d;
const MTypeId LerpVector::kTypeId = 0x0013107e;
const MTypeId SlerpQuaternion::kTypeId = 0x0013107f;
// Project Block5 IDs : [0x00131080 - 0x0013108f]
const MTypeId Smoothstep::kTypeId = 0x00131080;
const MTypeId SmoothstepAngle::kTypeId = 0x00131081;
const MTypeId SmoothstepMatrix::kTypeId = 0x00131082;
const MTypeId SmoothstepVector::kTypeId = 0x00131083;

// ------ LOGIC ------
const MTypeId AndBool::kTypeId = 0x00131084;
const MTypeId AndInt::kTypeId = 0x00131085;
const MTypeId NotBool::kTypeId = 0x00131086;
const MTypeId NotInt::kTypeId = 0x00131087;
const MTypeId OrBool::kTypeId = 0x00131088;
const MTypeId OrInt::kTypeId = 0x00131089;
const MTypeId XorBool::kTypeId = 0x0013108a;
const MTypeId XorInt::kTypeId = 0x0013108b;

// ------ MATRIX ------
const MTypeId AverageMatrix::kTypeId = 0x0013108c;
const MTypeId ComposeMatrix::kTypeId = 0x0013108d;
const MTypeId DecomposeMatrix::kTypeId = 0x0013108e;
const MTypeId ExtractBasisXFromMatrix::kTypeId = 0x0013108f;
// Project Block6 IDs : [0x00131090 - 0x0013109f]
const MTypeId ExtractBasisYFromMatrix::kTypeId = 0x00131090;
const MTypeId ExtractBasisZFromMatrix::kTypeId = 0x00131091;
const MTypeId ExtractEulerFromMatrix::kTypeId = 0x00131092;
const MTypeId ExtractQuaternionFromMatrix::kTypeId = 0x00131093;
const MTypeId ExtractRotationMatrix::kTypeId = 0x00131094;
const MTypeId ExtractScaleFromMatrix::kTypeId = 0x00131095;
const MTypeId ExtractScaleMatrix::kTypeId = 0x00131096;
const MTypeId ExtractTranslationFromMatrix::kTypeId = 0x00131097;
const MTypeId ExtractTranslationMatrix::kTypeId = 0x00131098;
const MTypeId MultiplyMatrix::kTypeId = 0x00131099;
const MTypeId MultiplyMatrixArray::kTypeId = 0x0013109a;
const MTypeId PostMultiplyMatrixByAxisXAngle::kTypeId = 0x0013109b;
const MTypeId PostMultiplyMatrixByAxisYAngle::kTypeId = 0x0013109c;
const MTypeId PostMultiplyMatrixByAxisZAngle::kTypeId = 0x0013109d;
const MTypeId PostMultiplyMatrixByEuler::kTypeId = 0x0013109e;
const MTypeId PostMultiplyMatrixByQuaternion::kTypeId = 0x0013109f;
// Project Block7 IDs : [0x001310a0 - 0x001310af]
const MTypeId PostMultiplyMatrixByScale::kTypeId = 0x001310a0;
const MTypeId PostMultiplyMatrixByTranslation::kTypeId = 0x001310a1;
const MTypeId PreMultiplyMatrixByAxisXAngle::kTypeId = 0x001310a2;
const MTypeId PreMultiplyMatrixByAxisYAngle::kTypeId = 0x001310a3;
const MTypeId PreMultiplyMatrixByAxisZAngle::kTypeId = 0x001310a4;
const MTypeId PreMultiplyMatrixByEuler::kTypeId = 0x001310a5;
const MTypeId PreMultiplyMatrixByQuaternion::kTypeId = 0x001310a6;
const MTypeId PreMultiplyMatrixByScale::kTypeId = 0x001310a7;
const MTypeId PreMultiplyMatrixByTranslation::kTypeId = 0x001310a8;
const MTypeId WeightedAverageMatrix::kTypeId = 0x001310a9;

// ------ POWER ------
const MTypeId Power::kTypeId = 0x001310aa;
const MTypeId SquareRoot::kTypeId = 0x001310ab;

// ------ QUATERNION ------
const MTypeId AverageQuaternion::kTypeId = 0x001310ac;
const MTypeId ConvertQuaternionToEuler::kTypeId = 0x001310ad;
const MTypeId ConvertQuaternionToMatrix::kTypeId = 0x001310ae;
const MTypeId MultiplyQuaternion::kTypeId = 0x001310af;
// Project Block8 IDs : [0x001310b0 - 0x001310bf]
const MTypeId MultiplyQuaternionArray::kTypeId = 0x001310b0;
const MTypeId WeightedAverageQuaternion::kTypeId = 0x001310b1;

// ------ RANGE ------
const MTypeId Clamp::kTypeId = 0x001310b2;
const MTypeId ClampAngle::kTypeId = 0x001310b3;
const MTypeId ClampInt::kTypeId = 0x001310b4;
const MTypeId Remap::kTypeId = 0x001310b5;
const MTypeId RemapAngle::kTypeId = 0x001310b6;
const MTypeId RemapInt::kTypeId = 0x001310b7;

// ------ ROUND ------
const MTypeId Ceil::kTypeId = 0x001310b8;
const MTypeId CeilAngle::kTypeId = 0x001310b9;
const MTypeId Floor::kTypeId = 0x001310ba;
const MTypeId FloorAngle::kTypeId = 0x001310bb;
const MTypeId Round::kTypeId = 0x001310bc;
const MTypeId RoundAngle::kTypeId = 0x001310bd;

// ------ TRIGONOMETRY ------
const MTypeId Acos::kTypeId = 0x001310be;
const MTypeId Asin::kTypeId = 0x001310bf;
// Project Block9 IDs : [0x001310c0 - 0x001310cf]
const MTypeId Atan::kTypeId = 0x001310c0;
const MTypeId Atan2::kTypeId = 0x001310c1;
const MTypeId Cos::kTypeId = 0x001310c2;
const MTypeId Sin::kTypeId = 0x001310c3;
const MTypeId Tan::kTypeId = 0x001310c4;

// ------ VECTOR ------
const MTypeId AddVector::kTypeId = 0x001310c5;
const MTypeId AverageVector::kTypeId = 0x001310c6;
const MTypeId ClosestPointOnLine::kTypeId = 0x001310c7;
const MTypeId ClosestPointOnPlane::kTypeId = 0x001310c8;
const MTypeId CrossProduct::kTypeId = 0x001310c9;
const MTypeId DivideVector::kTypeId = 0x001310ca;
const MTypeId DotProduct::kTypeId = 0x001310cb;
const MTypeId LimitVector::kTypeId = 0x001310cc;
const MTypeId MultiplyPointByMatrix::kTypeId = 0x001310cd;
const MTypeId MultiplyVector::kTypeId = 0x001310ce;
const MTypeId MultiplyVectorByMatrix::kTypeId = 0x001310cf;
// Project Block10 IDs : [0x001310d0 - 0x001310df]
const MTypeId NegateVector::kTypeId = 0x001310d0;
const MTypeId NormalizeVector::kTypeId = 0x001310d1;
const MTypeId ReflectVector::kTypeId = 0x001310d2;
const MTypeId RotateVectorByAxisAngle::kTypeId = 0x001310d3;
const MTypeId RotateVectorByEuler::kTypeId = 0x001310d4;
const MTypeId RotateVectorByQuaternion::kTypeId = 0x001310d5;
const MTypeId SubtractVector::kTypeId = 0x001310d6;
const MTypeId SumVector::kTypeId = 0x001310d7;
const MTypeId VectorLength::kTypeId = 0x001310d8;
const MTypeId VectorLengthSquared::kTypeId = 0x001310d9;
const MTypeId VectorPlaneProjection::kTypeId = 0x001310da;
const MTypeId VectorProjection::kTypeId = 0x001310db;
const MTypeId WeightedAverageVector::kTypeId = 0x001310dc;

// |---------|
// |  1.1.0  |
// |---------|

// ------ VECTOR ------
const MTypeId AngleBetweenVectors::kTypeId = 0x001310dd;
const MTypeId SignedAngleBetweenVectors::kTypeId = 0x001310de;

// ------ Trigonometry ------
const MTypeId PolarToCartesian::kTypeId = 0x001310df;
// Project Block11 IDs : [0x001310e0 - 0x001310ef]
const MTypeId CartesianToPolar::kTypeId = 0x001310e0;


// ------ kTypeName ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------ ARITHMETIC ------
const MString Add::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Add";
const MString AddAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AddAngle";
const MString AddInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AddInt";
const MString Average::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Average";
const MString AverageAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AverageAngle";
const MString AverageInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AverageInt";
const MString Divide::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Divide";
const MString DivideByInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "DivideByInt";
const MString DivideAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "DivideAngle";
const MString DivideAngleByInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "DivideAngleByInt";
const MString DivideInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "DivideInt";
const MString Modulus::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Modulus";
const MString Multiply::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Multiply";
const MString MultiplyByInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MultiplyByInt";
const MString MultiplyAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MultiplyAngle";
const MString MultiplyAngleByInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MultiplyAngleByInt";
const MString MultiplyInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MultiplyInt";
const MString Negate::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Negate";
const MString NegateAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "NegateAngle";
const MString NegateInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "NegateInt";
const MString NormalizeDoubleArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "NormalizeDoubleArray";
const MString NormalizeWeightArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "NormalizeWeightArray";
const MString Reciprocal::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Reciprocal";
const MString Subtract::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Subtract";
const MString SubtractAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SubtractAngle";
const MString SubtractInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SubtractInt";
const MString Sum::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Sum";
const MString SumAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SumAngle";
const MString SumInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SumInt";
const MString WeightedAverage::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "WeightedAverage";
const MString WeightedAverageAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "WeightedAverageAngle";
const MString WeightedAverageInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "WeightedAverageInt";

// ------ CONDITION ------
const MString Condition::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Condition";
const MString ConditionAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ConditionAngle";
const MString ConditionInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ConditionInt";
const MString Max::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Max";
const MString MaxAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MaxAngle";
const MString MaxInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MaxInt";
const MString MaxElement::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MaxElement";
const MString MaxElementAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MaxElementAngle";
const MString MaxElementInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MaxElementInt";
const MString Min::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Min";
const MString MinAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MinAngle";
const MString MinInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MinInt";
const MString MinElement::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MinElement";
const MString MinElementAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MinElementAngle";
const MString MinElementInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MinElementInt";
const MString Select::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Select";
const MString SelectDoubleArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectDoubleArray";
const MString SelectAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectAngle";
const MString SelectAngleArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectAngleArray";
const MString SelectCurve::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectCurve";
const MString SelectEuler::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectEuler";
const MString SelectEulerArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectEulerArray";
const MString SelectInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectInt";
const MString SelectIntArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectIntArray";
const MString SelectMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectMatrix";
const MString SelectMatrixArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectMatrixArray";
const MString SelectMesh::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectMesh";
const MString SelectQuaternion::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectQuaternion";
const MString SelectQuaternionArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectQuaternionArray";
const MString SelectSurface::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectSurface";
const MString SelectVector::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectVector";
const MString SelectVectorArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SelectVectorArray";

// ------ DISTANCE ------
const MString Absolute::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Absolute";
const MString AbsoluteAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AbsoluteAngle";
const MString AbsoluteInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AbsoluteInt";
const MString DistanceBetweenFrames::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "DistanceBetweenFrames";
const MString DistanceBetweenPoints::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "DistanceBetweenPoints";

// ------ EULER ------
const MString AverageEuler::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AverageEuler";
const MString ConvertEulerToMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ConvertEulerToMatrix";
const MString ConvertEulerToQuaternion::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ConvertEulerToQuaternion";
const MString DivideEuler::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "DivideEuler";
const MString MultiplyEuler::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MultiplyEuler";
const MString WeightedAverageEuler::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "WeightedAverageEuler";

// ------ INTERPOLATE ------
const MString Lerp::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Lerp";
const MString LerpAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "LerpAngle";
const MString LerpMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "LerpMatrix";
const MString LerpVector::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "LerpVector";
const MString SlerpQuaternion::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SlerpQuaternion";
const MString Smoothstep::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Smoothstep";
const MString SmoothstepAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SmoothstepAngle";
const MString SmoothstepMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SmoothstepMatrix";
const MString SmoothstepVector::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SmoothstepVector";

// ------ LOGIC ------
const MString AndBool::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AndBool";
const MString AndInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AndInt";
const MString NotBool::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "NotBool";
const MString NotInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "NotInt";
const MString OrBool::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "OrBool";
const MString OrInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "OrInt";
const MString XorBool::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "XorBool";
const MString XorInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "XorInt";

// ------ MATRIX ------
const MString AverageMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AverageMatrix";
const MString ComposeMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ComposeMatrix";
const MString DecomposeMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "DecomposeMatrix";
const MString ExtractBasisXFromMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ExtractBasisXFromMatrix";
const MString ExtractBasisYFromMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ExtractBasisYFromMatrix";
const MString ExtractBasisZFromMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ExtractBasisZFromMatrix";
const MString ExtractEulerFromMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ExtractEulerFromMatrix";
const MString ExtractQuaternionFromMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ExtractQuaternionFromMatrix";
const MString ExtractRotationMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ExtractRotationMatrix";
const MString ExtractScaleFromMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ExtractScaleFromMatrix";
const MString ExtractScaleMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ExtractScaleMatrix";
const MString ExtractTranslationFromMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ExtractTranslationFromMatrix";
const MString ExtractTranslationMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ExtractTranslationMatrix";
const MString MultiplyMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MultiplyMatrix";
const MString MultiplyMatrixArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MultiplyMatrixArray";
const MString PostMultiplyMatrixByAxisXAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PostMultiplyMatrixByAxisXAngle";
const MString PostMultiplyMatrixByAxisYAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PostMultiplyMatrixByAxisYAngle";
const MString PostMultiplyMatrixByAxisZAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PostMultiplyMatrixByAxisZAngle";
const MString PostMultiplyMatrixByEuler::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PostMultiplyMatrixByEuler";
const MString PostMultiplyMatrixByQuaternion::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PostMultiplyMatrixByQuaternion";
const MString PostMultiplyMatrixByScale::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PostMultiplyMatrixByScale";
const MString PostMultiplyMatrixByTranslation::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PostMultiplyMatrixByTranslation";
const MString PreMultiplyMatrixByAxisXAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PreMultiplyMatrixByAxisXAngle";
const MString PreMultiplyMatrixByAxisYAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PreMultiplyMatrixByAxisYAngle";
const MString PreMultiplyMatrixByAxisZAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PreMultiplyMatrixByAxisZAngle";
const MString PreMultiplyMatrixByEuler::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PreMultiplyMatrixByEuler";
const MString PreMultiplyMatrixByQuaternion::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PreMultiplyMatrixByQuaternion";
const MString PreMultiplyMatrixByScale::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PreMultiplyMatrixByScale";
const MString PreMultiplyMatrixByTranslation::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PreMultiplyMatrixByTranslation";
const MString WeightedAverageMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "WeightedAverageMatrix";

// ------ POWER ------
const MString Power::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Power";
const MString SquareRoot::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SquareRoot";

// ------ QUATERNION ------
const MString AverageQuaternion::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AverageQuaternion";
const MString ConvertQuaternionToEuler::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ConvertQuaternionToEuler";
const MString ConvertQuaternionToMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ConvertQuaternionToMatrix";
const MString MultiplyQuaternion::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MultiplyQuaternion";
const MString MultiplyQuaternionArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MultiplyQuaternionArray";
const MString WeightedAverageQuaternion::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "WeightedAverageQuaternion";

// ------ RANGE ------
const MString Clamp::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Clamp";
const MString ClampAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ClampAngle";
const MString ClampInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ClampInt";
const MString Remap::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Remap";
const MString RemapAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "RemapAngle";
const MString RemapInt::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "RemapInt";

// ------ ROUND ------
const MString Ceil::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Ceil";
const MString CeilAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "CeilAngle";
const MString Floor::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Floor";
const MString FloorAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "FloorAngle";
const MString Round::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Round";
const MString RoundAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "RoundAngle";

// ------ TRIGONOMETRY ------
const MString Acos::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Acos";
const MString Asin::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Asin";
const MString Atan::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Atan";
const MString Atan2::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Atan2";
const MString CartesianToPolar::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "CartesianToPolar";
const MString Cos::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Cos";
const MString PolarToCartesian::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PolarToCartesian";
const MString Sin::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Sin";
const MString Tan::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Tan";

// ------ VECTOR ------
const MString AddVector::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AddVector";
const MString AverageVector::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AverageVector";
const MString ClosestPointOnLine::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ClosestPointOnLine";
const MString ClosestPointOnPlane::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ClosestPointOnPlane";
const MString CrossProduct::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "CrossProduct";
const MString DivideVector::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "DivideVector";
const MString DotProduct::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "DotProduct";
const MString LimitVector::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "LimitVector";
const MString MultiplyPointByMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MultiplyPointByMatrix";
const MString MultiplyVector::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MultiplyVector";
const MString MultiplyVectorByMatrix::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "MultiplyVectorByMatrix";
const MString NegateVector::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "NegateVector";
const MString NormalizeVector::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "NormalizeVector";
const MString ReflectVector::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ReflectVector";
const MString RotateVectorByAxisAngle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "RotateVectorByAxisAngle";
const MString RotateVectorByEuler::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "RotateVectorByEuler";
const MString RotateVectorByQuaternion::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "RotateVectorByQuaternion";
const MString SubtractVector::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SubtractVector";
const MString SumVector::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SumVector";
const MString VectorLength::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "VectorLength";
const MString VectorLengthSquared::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "VectorLengthSquared";
const MString VectorPlaneProjection::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "VectorPlaneProjection";
const MString VectorProjection::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "VectorProjection";
const MString WeightedAverageVector::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "WeightedAverageVector";
const MString AngleBetweenVectors::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AngleBetweenVectors";
const MString SignedAngleBetweenVectors::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "SignedAngleBetweenVectors";


// ------ Exports -----------------------------------------------------------------------------------------------------------------------------------------------------------

#define kErrorInvalidPluginId \
	"Plugin \"^1s\" has been given an invalid id"

#define kErrorPluginRegistration \
	"Error registering plugin \"^1s\""

#define kErrorPluginDeregistration \
	"Error deregistering plugin \"^1s\""

MStatus initializePlugin(MObject plugin)
{
	MString errorMessage;

	MFnPlugin fnPlugin(plugin, "Mitchell Simmons", MRS_XSTR(PROJECT_VERSION), "Any");
	fnPlugin.setName(MRS_XSTR(TARGET_NAME));

	// |------------|
	// |  Validate  |
	// |------------|

	// ------ ARITHMETIC ------
	errorMessage.format(kErrorInvalidPluginId, Add::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Add::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, AddAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AddAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, AddInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AddInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Average::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Average::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, AverageAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AverageAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, AverageInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AverageInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Divide::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Divide::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, DivideByInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(DivideByInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, DivideAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(DivideAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, DivideAngleByInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(DivideAngleByInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, DivideInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(DivideInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Modulus::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Modulus::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Multiply::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Multiply::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MultiplyByInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MultiplyByInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MultiplyAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MultiplyAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MultiplyAngleByInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MultiplyAngleByInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MultiplyInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MultiplyInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Negate::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Negate::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, NegateAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(NegateAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, NegateInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(NegateInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, NormalizeDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(NormalizeDoubleArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, NormalizeWeightArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(NormalizeWeightArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Reciprocal::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Reciprocal::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Subtract::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Subtract::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SubtractAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SubtractAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SubtractInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SubtractInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Sum::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Sum::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SumAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SumAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SumInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SumInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, WeightedAverage::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(WeightedAverage::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// ------ CONDITION ------
	errorMessage.format(kErrorInvalidPluginId, Condition::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Condition::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ConditionAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ConditionAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ConditionInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ConditionInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Max::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Max::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MaxAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MaxAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MaxInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MaxInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MaxElement::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MaxElement::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MaxElementAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MaxElementAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MaxElementInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MaxElementInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Min::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Min::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MinAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MinAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MinInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MinInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MinElement::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MinElement::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MinElementAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MinElementAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MinElementInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MinElementInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Select::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Select::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectDoubleArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectAngleArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectCurve::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectCurve::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectEuler::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectEulerArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectIntArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectMatrixArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectMesh::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectMesh::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectQuaternion::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectQuaternionArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectQuaternionArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectSurface::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectSurface::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectVector::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SelectVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SelectVectorArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// ------ DISTANCE ------
	errorMessage.format(kErrorInvalidPluginId, Absolute::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Absolute::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, AbsoluteAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AbsoluteAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, AbsoluteInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AbsoluteInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, DistanceBetweenFrames::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(DistanceBetweenFrames::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, DistanceBetweenPoints::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(DistanceBetweenPoints::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// ------ EULER ------
	errorMessage.format(kErrorInvalidPluginId, AverageEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AverageEuler::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ConvertEulerToMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ConvertEulerToMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ConvertEulerToQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ConvertEulerToQuaternion::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, DivideEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(DivideEuler::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MultiplyEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MultiplyEuler::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, WeightedAverageEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(WeightedAverageEuler::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// ------ INTERPOLATE ------
	errorMessage.format(kErrorInvalidPluginId, Lerp::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Lerp::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, LerpAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(LerpAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, LerpMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(LerpMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, LerpVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(LerpVector::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SlerpQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SlerpQuaternion::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Smoothstep::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Smoothstep::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SmoothstepAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SmoothstepAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SmoothstepMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SmoothstepMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SmoothstepVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SmoothstepVector::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// ------ LOGIC ------
	errorMessage.format(kErrorInvalidPluginId, AndBool::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AndBool::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, AndInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AndInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, NotBool::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(NotBool::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, NotInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(NotInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, OrBool::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(OrBool::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, OrInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(OrInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, XorBool::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(XorBool::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, XorInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(XorInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// ------ MATRIX ------
	errorMessage.format(kErrorInvalidPluginId, AverageMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AverageMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ComposeMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ComposeMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, DecomposeMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(DecomposeMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ExtractBasisXFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ExtractBasisXFromMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ExtractBasisYFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ExtractBasisYFromMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ExtractBasisZFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ExtractBasisZFromMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ExtractEulerFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ExtractEulerFromMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ExtractRotationMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ExtractRotationMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ExtractQuaternionFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ExtractQuaternionFromMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ExtractScaleFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ExtractScaleFromMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ExtractScaleMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ExtractScaleMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ExtractTranslationFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ExtractTranslationFromMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ExtractTranslationMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ExtractTranslationMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MultiplyMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MultiplyMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MultiplyMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MultiplyMatrixArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PostMultiplyMatrixByAxisXAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PostMultiplyMatrixByAxisXAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PostMultiplyMatrixByAxisYAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PostMultiplyMatrixByAxisYAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PostMultiplyMatrixByAxisZAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PostMultiplyMatrixByAxisZAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PostMultiplyMatrixByEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PostMultiplyMatrixByEuler::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PostMultiplyMatrixByQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PostMultiplyMatrixByQuaternion::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PostMultiplyMatrixByScale::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PostMultiplyMatrixByScale::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PostMultiplyMatrixByTranslation::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PostMultiplyMatrixByTranslation::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PreMultiplyMatrixByAxisXAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PreMultiplyMatrixByAxisXAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PreMultiplyMatrixByAxisYAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PreMultiplyMatrixByAxisYAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PreMultiplyMatrixByAxisZAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PreMultiplyMatrixByAxisZAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PreMultiplyMatrixByEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PreMultiplyMatrixByEuler::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PreMultiplyMatrixByQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PreMultiplyMatrixByQuaternion::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PreMultiplyMatrixByScale::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PreMultiplyMatrixByScale::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PreMultiplyMatrixByTranslation::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PreMultiplyMatrixByTranslation::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, WeightedAverageMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(WeightedAverageMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// ------ Power ------
	errorMessage.format(kErrorInvalidPluginId, Power::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Power::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SquareRoot::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SquareRoot::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// ------ QUATERNION ------
	errorMessage.format(kErrorInvalidPluginId, AverageQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AverageQuaternion::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ConvertQuaternionToEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ConvertQuaternionToEuler::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ConvertQuaternionToMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ConvertQuaternionToMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MultiplyQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MultiplyQuaternion::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MultiplyQuaternionArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MultiplyQuaternionArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, WeightedAverageQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(WeightedAverageQuaternion::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// ------ RANGE ------
	errorMessage.format(kErrorInvalidPluginId, Clamp::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Clamp::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ClampAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ClampAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ClampInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ClampInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Remap::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Remap::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, RemapAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(RemapAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, RemapInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(RemapInt::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// ------ ROUND ------
	errorMessage.format(kErrorInvalidPluginId, Ceil::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Ceil::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, CeilAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(CeilAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Floor::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Floor::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FloorAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FloorAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Round::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Round::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, RoundAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(RoundAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// ------ TRIGONOMETRY ------
	errorMessage.format(kErrorInvalidPluginId, Acos::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Acos::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Asin::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Asin::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Atan::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Atan::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Atan2::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Atan2::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, CartesianToPolar::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(CartesianToPolar::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Cos::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Cos::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PolarToCartesian::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PolarToCartesian::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Sin::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Sin::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Tan::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Tan::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// ------ VECTOR ------
	errorMessage.format(kErrorInvalidPluginId, AddVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AddVector::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, AngleBetweenVectors::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AngleBetweenVectors::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, AverageVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AverageVector::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ClosestPointOnLine::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ClosestPointOnLine::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ClosestPointOnPlane::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ClosestPointOnPlane::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, CrossProduct::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(CrossProduct::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, DivideVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(DivideVector::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, DotProduct::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(DotProduct::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, LimitVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(LimitVector::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MultiplyPointByMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MultiplyPointByMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MultiplyVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MultiplyVector::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MultiplyVectorByMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(MultiplyVectorByMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, NegateVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(NegateVector::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, NormalizeVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(NormalizeVector::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ReflectVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ReflectVector::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, RotateVectorByAxisAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(RotateVectorByAxisAngle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, RotateVectorByEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(RotateVectorByEuler::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, RotateVectorByQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(RotateVectorByQuaternion::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SignedAngleBetweenVectors::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SignedAngleBetweenVectors::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SubtractVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SubtractVector::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, SumVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(SumVector::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, VectorLength::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(VectorLength::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, VectorLengthSquared::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(VectorLengthSquared::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, VectorPlaneProjection::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(VectorPlaneProjection::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, VectorProjection::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(VectorProjection::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, WeightedAverageVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(WeightedAverageVector::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// |------------|
	// |  Register  |
	// |------------|

	// ------ ARITHMETIC ------
	errorMessage.format(kErrorPluginRegistration, Add::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Add>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, AddAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<AddAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, AddInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<AddInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Average::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Average>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, AverageAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<AverageAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, AverageInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<AverageInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Divide::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Divide>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, DivideByInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<DivideByInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, DivideAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<DivideAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, DivideAngleByInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<DivideAngleByInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, DivideInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<DivideInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Modulus::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Modulus>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Multiply::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Multiply>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MultiplyByInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MultiplyByInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MultiplyAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MultiplyAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MultiplyAngleByInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MultiplyAngleByInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MultiplyInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MultiplyInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Negate::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Negate>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, NegateAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<NegateAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, NegateInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<NegateInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, NormalizeDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<NormalizeDoubleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, NormalizeWeightArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<NormalizeWeightArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Reciprocal::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode <Reciprocal>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Subtract::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Subtract>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SubtractAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SubtractAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SubtractInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SubtractInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Sum::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Sum>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SumAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SumAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SumInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SumInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, WeightedAverage::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<WeightedAverage>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, WeightedAverageAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<WeightedAverageAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, WeightedAverageInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<WeightedAverageInt>(fnPlugin), errorMessage);

	// ------ CONDITION ------
	errorMessage.format(kErrorPluginRegistration, Condition::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Condition>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ConditionAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ConditionAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ConditionInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ConditionInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Max::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Max>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MaxAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MaxAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MaxInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MaxInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MaxElement::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MaxElement>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MaxElementAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MaxElementAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MaxElementInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MaxElementInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Min::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Min>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MinAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MinAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MinInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MinInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MinElement::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MinElement>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MinElementAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MinElementAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MinElementInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MinElementInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Select::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Select>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectDoubleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectAngleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectCurve::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectCurve>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectEulerArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectIntArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectMatrixArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectMesh::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectMesh>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectQuaternionArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectQuaternionArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectSurface::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectSurface>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SelectVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SelectVectorArray>(fnPlugin), errorMessage);

	// ------ DISTANCE ------
	errorMessage.format(kErrorPluginRegistration, Absolute::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Absolute>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, AbsoluteAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<AbsoluteAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, AbsoluteInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<AbsoluteInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, DistanceBetweenFrames::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<DistanceBetweenFrames>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, DistanceBetweenPoints::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<DistanceBetweenPoints>(fnPlugin), errorMessage);

	// ------ EULER ------
	errorMessage.format(kErrorPluginRegistration, AverageEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<AverageEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ConvertEulerToMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ConvertEulerToMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ConvertEulerToQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ConvertEulerToQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, DivideEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<DivideEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MultiplyEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MultiplyEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, WeightedAverageEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<WeightedAverageEuler>(fnPlugin), errorMessage);

	// ------ INTERPOLATE ------
	errorMessage.format(kErrorPluginRegistration, Lerp::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Lerp>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, LerpAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<LerpAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, LerpMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<LerpMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, LerpVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<LerpVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SlerpQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SlerpQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Smoothstep::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Smoothstep>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SmoothstepAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SmoothstepAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SmoothstepMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SmoothstepMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SmoothstepVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SmoothstepVector>(fnPlugin), errorMessage);

	// ------ LOGIC ------
	errorMessage.format(kErrorPluginRegistration, AndBool::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<AndBool>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, AndInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<AndInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, NotBool::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<NotBool>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, NotInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<NotInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, OrBool::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<OrBool>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, OrInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<OrInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, XorBool::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<XorBool>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, XorInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<XorInt>(fnPlugin), errorMessage);

	// ------ MATRIX ------
	errorMessage.format(kErrorPluginRegistration, AverageMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<AverageMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ComposeMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ComposeMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, DecomposeMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<DecomposeMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ExtractBasisXFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ExtractBasisXFromMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ExtractBasisYFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ExtractBasisYFromMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ExtractBasisZFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ExtractBasisZFromMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ExtractEulerFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ExtractEulerFromMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ExtractRotationMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ExtractRotationMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ExtractQuaternionFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ExtractQuaternionFromMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ExtractScaleFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ExtractScaleFromMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ExtractScaleMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ExtractScaleMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ExtractTranslationFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ExtractTranslationFromMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ExtractTranslationMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ExtractTranslationMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MultiplyMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MultiplyMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MultiplyMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MultiplyMatrixArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PostMultiplyMatrixByAxisXAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PostMultiplyMatrixByAxisXAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PostMultiplyMatrixByAxisYAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PostMultiplyMatrixByAxisYAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PostMultiplyMatrixByAxisZAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PostMultiplyMatrixByAxisZAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PostMultiplyMatrixByEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PostMultiplyMatrixByEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PostMultiplyMatrixByQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PostMultiplyMatrixByQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PostMultiplyMatrixByScale::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PostMultiplyMatrixByScale>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PostMultiplyMatrixByTranslation::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PostMultiplyMatrixByTranslation>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PreMultiplyMatrixByAxisXAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PreMultiplyMatrixByAxisXAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PreMultiplyMatrixByAxisYAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PreMultiplyMatrixByAxisYAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PreMultiplyMatrixByAxisZAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PreMultiplyMatrixByAxisZAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PreMultiplyMatrixByEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PreMultiplyMatrixByEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PreMultiplyMatrixByQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PreMultiplyMatrixByQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PreMultiplyMatrixByScale::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PreMultiplyMatrixByScale>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PreMultiplyMatrixByTranslation::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PreMultiplyMatrixByTranslation>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, WeightedAverageMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<WeightedAverageMatrix>(fnPlugin), errorMessage);

	// ------ POWER ------
	errorMessage.format(kErrorPluginRegistration, Power::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Power>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SquareRoot::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SquareRoot>(fnPlugin), errorMessage);

	// ------ QUATERNION ------
	errorMessage.format(kErrorPluginRegistration, AverageQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<AverageQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ConvertQuaternionToEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ConvertQuaternionToEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ConvertQuaternionToMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ConvertQuaternionToMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MultiplyQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MultiplyQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MultiplyQuaternionArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MultiplyQuaternionArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, WeightedAverageQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<WeightedAverageQuaternion>(fnPlugin), errorMessage);

	// ------ RANGE ------
	errorMessage.format(kErrorPluginRegistration, Clamp::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Clamp>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ClampAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ClampAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ClampInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ClampInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Remap::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Remap>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, RemapAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<RemapAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, RemapInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<RemapInt>(fnPlugin), errorMessage);

	// ------ ROUND ------
	errorMessage.format(kErrorPluginRegistration, Ceil::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Ceil>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, CeilAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<CeilAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Floor::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Floor>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FloorAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<FloorAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Round::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Round>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, RoundAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<RoundAngle>(fnPlugin), errorMessage);

	// ------ TRIGONOMETRY ------
	errorMessage.format(kErrorPluginRegistration, Acos::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Acos>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Asin::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Asin>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Atan::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Atan>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Atan2::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Atan2>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, CartesianToPolar::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<CartesianToPolar>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Cos::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Cos>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PolarToCartesian::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PolarToCartesian>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Sin::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Sin>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, Tan::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Tan>(fnPlugin), errorMessage);

	// ------ VECTOR ------
	errorMessage.format(kErrorPluginRegistration, AddVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<AddVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, AngleBetweenVectors::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<AngleBetweenVectors>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, AverageVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<AverageVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ClosestPointOnLine::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ClosestPointOnLine>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ClosestPointOnPlane::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ClosestPointOnPlane>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, CrossProduct::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<CrossProduct>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, DivideVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<DivideVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, DotProduct::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<DotProduct>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, LimitVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<LimitVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MultiplyPointByMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MultiplyPointByMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MultiplyVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MultiplyVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, MultiplyVectorByMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<MultiplyVectorByMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, NegateVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<NegateVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, NormalizeVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<NormalizeVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ReflectVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ReflectVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, RotateVectorByAxisAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<RotateVectorByAxisAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, RotateVectorByEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<RotateVectorByEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, RotateVectorByQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<RotateVectorByQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SignedAngleBetweenVectors::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SignedAngleBetweenVectors>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SubtractVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SubtractVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, SumVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<SumVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, VectorLength::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<VectorLength>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, VectorLengthSquared::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<VectorLengthSquared>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, VectorPlaneProjection::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<VectorPlaneProjection>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, VectorProjection::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<VectorProjection>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, WeightedAverageVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<WeightedAverageVector>(fnPlugin), errorMessage);

	return MStatus::kSuccess;
}

MStatus uninitializePlugin(MObject plugin) 
{
	MString errorMessage;

	MFnPlugin fnPlugin(plugin);

	// |--------------|
	// |  Deregister  |
	// |--------------|

	// ------ ARITHMETIC ------
	errorMessage.format(kErrorPluginDeregistration, Add::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Add>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, AddAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AddAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, AddInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AddInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Average::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Average>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, AverageAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AverageAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, AverageInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AverageInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Divide::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Divide>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, DivideByInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<DivideByInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, DivideAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<DivideAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, DivideAngleByInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<DivideAngleByInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, DivideInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<DivideInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Modulus::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Modulus>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Multiply::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Multiply>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MultiplyByInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MultiplyByInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MultiplyAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MultiplyAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MultiplyAngleByInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MultiplyAngleByInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MultiplyInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MultiplyInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Negate::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Negate>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, NegateAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<NegateAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, NegateInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<NegateInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, NormalizeDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<NormalizeDoubleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, NormalizeWeightArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<NormalizeWeightArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Reciprocal::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode <Reciprocal>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Subtract::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Subtract>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SubtractAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SubtractAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SubtractInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SubtractInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Sum::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Sum>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SumAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SumAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SumInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SumInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, WeightedAverage::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<WeightedAverage>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, WeightedAverageAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<WeightedAverageAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, WeightedAverageInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<WeightedAverageInt>(fnPlugin), errorMessage);

	// ------ CONDITION ------
	errorMessage.format(kErrorPluginDeregistration, Condition::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Condition>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ConditionAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ConditionAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ConditionInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ConditionInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Max::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Max>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MaxAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MaxAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MaxInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MaxInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MaxElement::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MaxElement>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MaxElementAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MaxElementAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MaxElementInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MaxElementInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Min::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Min>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MinAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MinAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MinInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MinInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MinElement::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MinElement>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MinElementAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MinElementAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MinElementInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MinElementInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Select::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Select>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectDoubleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectAngleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectCurve::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectCurve>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectEulerArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectIntArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectMatrixArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectMesh::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectMesh>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectQuaternionArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectQuaternionArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectSurface::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectSurface>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SelectVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SelectVectorArray>(fnPlugin), errorMessage);

	// ------ DISTANCE ------
	errorMessage.format(kErrorPluginDeregistration, Absolute::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Absolute>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, AbsoluteAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AbsoluteAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, AbsoluteInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AbsoluteInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, DistanceBetweenFrames::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<DistanceBetweenFrames>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, DistanceBetweenPoints::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<DistanceBetweenPoints>(fnPlugin), errorMessage);

	// ------ EULER ------
	errorMessage.format(kErrorPluginDeregistration, AverageEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AverageEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ConvertEulerToMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ConvertEulerToMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ConvertEulerToQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ConvertEulerToQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, DivideEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<DivideEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MultiplyEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MultiplyEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, WeightedAverageEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<WeightedAverageEuler>(fnPlugin), errorMessage);

	// ------ INTERPOLATE ------
	errorMessage.format(kErrorPluginDeregistration, Lerp::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Lerp>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, LerpAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<LerpAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, LerpMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<LerpMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, LerpVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<LerpVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SlerpQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SlerpQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Smoothstep::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Smoothstep>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SmoothstepAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SmoothstepAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SmoothstepMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SmoothstepMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SmoothstepVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SmoothstepVector>(fnPlugin), errorMessage);

	// ------ LOGIC ------
	errorMessage.format(kErrorPluginDeregistration, AndBool::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AndBool>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, AndInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AndInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, NotBool::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<NotBool>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, NotInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<NotInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, OrBool::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<OrBool>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, OrInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<OrInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, XorBool::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<XorBool>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, XorInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<XorInt>(fnPlugin), errorMessage);

	// ------ MATRIX ------
	errorMessage.format(kErrorPluginDeregistration, AverageMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AverageMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ComposeMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ComposeMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, DecomposeMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<DecomposeMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ExtractBasisXFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ExtractBasisXFromMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ExtractBasisYFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ExtractBasisYFromMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ExtractBasisZFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ExtractBasisZFromMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ExtractEulerFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ExtractEulerFromMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ExtractRotationMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ExtractRotationMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ExtractQuaternionFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ExtractQuaternionFromMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ExtractScaleFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ExtractScaleFromMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ExtractScaleMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ExtractScaleMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ExtractTranslationFromMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ExtractTranslationFromMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ExtractTranslationMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ExtractTranslationMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MultiplyMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MultiplyMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MultiplyMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MultiplyMatrixArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PostMultiplyMatrixByAxisXAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PostMultiplyMatrixByAxisXAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PostMultiplyMatrixByAxisYAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PostMultiplyMatrixByAxisYAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PostMultiplyMatrixByAxisZAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PostMultiplyMatrixByAxisZAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PostMultiplyMatrixByEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PostMultiplyMatrixByEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PostMultiplyMatrixByQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PostMultiplyMatrixByQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PostMultiplyMatrixByScale::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PostMultiplyMatrixByScale>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PostMultiplyMatrixByTranslation::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PostMultiplyMatrixByTranslation>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PreMultiplyMatrixByAxisXAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PreMultiplyMatrixByAxisXAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PreMultiplyMatrixByAxisYAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PreMultiplyMatrixByAxisYAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PreMultiplyMatrixByAxisZAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PreMultiplyMatrixByAxisZAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PreMultiplyMatrixByEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PreMultiplyMatrixByEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PreMultiplyMatrixByQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PreMultiplyMatrixByQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PreMultiplyMatrixByScale::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PreMultiplyMatrixByScale>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PreMultiplyMatrixByTranslation::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PreMultiplyMatrixByTranslation>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, WeightedAverageMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<WeightedAverageMatrix>(fnPlugin), errorMessage);

	// ------ POWER ------
	errorMessage.format(kErrorPluginDeregistration, Power::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Power>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SquareRoot::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SquareRoot>(fnPlugin), errorMessage);

	// ------ QUATERNION ------
	errorMessage.format(kErrorPluginDeregistration, AverageQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AverageQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ConvertQuaternionToEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ConvertQuaternionToEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ConvertQuaternionToMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ConvertQuaternionToMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MultiplyQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MultiplyQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MultiplyQuaternionArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MultiplyQuaternionArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, WeightedAverageQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<WeightedAverageQuaternion>(fnPlugin), errorMessage);

	// ------ RANGE ------
	errorMessage.format(kErrorPluginDeregistration, Clamp::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Clamp>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ClampAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ClampAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ClampInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ClampInt>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Remap::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Remap>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, RemapAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<RemapAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, RemapInt::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<RemapInt>(fnPlugin), errorMessage);

	// ------ ROUND ------
	errorMessage.format(kErrorPluginDeregistration, Ceil::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Ceil>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, CeilAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<CeilAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Floor::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Floor>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FloorAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FloorAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Round::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Round>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, RoundAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<RoundAngle>(fnPlugin), errorMessage);

	// ------ TRIGONOMETRY ------
	errorMessage.format(kErrorPluginDeregistration, Acos::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Acos>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Asin::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Asin>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Atan::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Atan>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Atan2::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Atan2>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, CartesianToPolar::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<CartesianToPolar>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Cos::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Cos>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PolarToCartesian::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PolarToCartesian>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Sin::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Sin>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, Tan::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Tan>(fnPlugin), errorMessage);

	// ------ VECTOR ------
	errorMessage.format(kErrorPluginDeregistration, AddVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AddVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, AngleBetweenVectors::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AngleBetweenVectors>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, AverageVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AverageVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ClosestPointOnLine::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ClosestPointOnLine>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ClosestPointOnPlane::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ClosestPointOnPlane>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, CrossProduct::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<CrossProduct>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, DivideVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<DivideVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, DotProduct::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<DotProduct>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, LimitVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<LimitVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MultiplyPointByMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MultiplyPointByMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MultiplyVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MultiplyVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, MultiplyVectorByMatrix::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<MultiplyVectorByMatrix>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, NegateVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<NegateVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, NormalizeVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<NormalizeVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ReflectVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ReflectVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, RotateVectorByAxisAngle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<RotateVectorByAxisAngle>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, RotateVectorByEuler::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<RotateVectorByEuler>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, RotateVectorByQuaternion::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<RotateVectorByQuaternion>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SignedAngleBetweenVectors::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SignedAngleBetweenVectors>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SubtractVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SubtractVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, SumVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<SumVector>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, VectorLength::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<VectorLength>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, VectorLengthSquared::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<VectorLengthSquared>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, VectorPlaneProjection::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<VectorPlaneProjection>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, VectorProjection::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<VectorProjection>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, WeightedAverageVector::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<WeightedAverageVector>(fnPlugin), errorMessage);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------