import os

# DIRECTORY = "C:\\dev\\maya\\msPlugins\\src\\nodes\\math\\arithmetic\\scripts\\templates"
# names = ["Add", "AddAngle", "AddInt", "Average", "AverageAngle", "AverageInt", "Divide", "DivideByInt", "DivideAngle", "DivideAngleByInt", "DivideInt", "Modulus", "Multiply", "MultiplyByInt", "MultiplyAngle", "MultiplyAngleByInt", "MultiplyInt", "Negate", "NegateAngle", "NegateInt", "NormalizeDoubleArray", "NormalizeWeightArray", "Reciprocal", "ReciprocalAngle", "Subtract", "SubtractAngle", "SubtractInt", "Sum", "SumAngle", "SumInt", "WeightedAverage", "WeightedAverageAngle", "WeightedAverageInt"]

# DIRECTORY = "C:\\dev\\maya\\msPlugins\\src\\nodes\\math\\condition\\scripts\\templates"
# names = ["Condition", "ConditionAngle", "ConditionInt", "Max", "MaxAngle", "MaxInt", "MaxElement", "MaxElementAngle", "MaxElementInt", "Min", "MinAngle", "MinInt", "MinElement", "MinElementAngle", "MinElementInt", "Select", "SelectDoubleArray", "SelectAngle", "SelectAngleArray", "SelectCurve", "SelectEuler", "SelectEulerArray", "SelectInt", "SelectIntArray", "SelectMatrix", "SelectMatrixArray", "SelectMesh", "SelectQuaternion", "SelectQuaternionArray", "SelectSurface", "SelectVector", "SelectVectorArray"]

# DIRECTORY = "C:\\dev\\maya\\msPlugins\\src\\nodes\\math\\distance\\scripts\\templates"
# names = ["Absolute", "AbsoluteAngle", "AbsoluteInt", "DistanceBetweenFrames", "DistanceBetweenPoints"]

# DIRECTORY = "C:\\dev\\maya\\msPlugins\\src\\nodes\\math\\euler\\scripts\\templates"
# names = ["AverageEuler", "ConvertEulerToMatrix", "ConvertEulerToQuaternion", "DivideEuler", "MultiplyEuler", "WeightedAverageEuler"]

# DIRECTORY = "C:\\dev\\maya\\msPlugins\\src\\nodes\\math\\interpolate\\scripts\\templates"
# names = ["Lerp", "LerpAngle", "LerpMatrix", "LerpVector", "SlerpQuaternion", "Smoothstep", "SmoothstepAngle", "SmoothstepMatrix", "SmoothstepVector"]

# DIRECTORY = "C:\\dev\\maya\\msPlugins\\src\\nodes\\math\\logic\\scripts\\templates"
# names = ["AndBool", "AndInt", "NotBool", "NotInt", "OrBool", "OrInt", "XorBool", "XorInt"]

# DIRECTORY = "C:\\dev\\maya\\msPlugins\\src\\nodes\\math\\matrix\\scripts\\templates"
# names = ["AverageMatrix", "ComposeMatrix", "DecomposeMatrix", "ExtractBasisXFromMatrix", "ExtractBasisYFromMatrix", "ExtractBasisZFromMatrix", "ExtractEulerFromMatrix", "ExtractQuaternionFromMatrix", "ExtractRotationMatrix", "ExtractScaleFromMatrix", "ExtractScaleMatrix", "ExtractTranslationFromMatrix", "ExtractTranslationMatrix", "MultiplyMatrix", "MultiplyMatrixArray", "PostMultiplyMatrixByAxisXAngle", "PostMultiplyMatrixByAxisYAngle", "PostMultiplyMatrixByAxisZAngle", "PostMultiplyMatrixByEuler", "PostMultiplyMatrixByQuaternion", "PostMultiplyMatrixByScale", "PostMultiplyMatrixByTranslation", "PreMultiplyMatrixByAxisXAngle", "PreMultiplyMatrixByAxisYAngle", "PreMultiplyMatrixByAxisZAngle", "PreMultiplyMatrixByEuler", "PreMultiplyMatrixByQuaternion", "PreMultiplyMatrixByScale", "PreMultiplyMatrixByTranslation", "WeightedAverageMatrix"]

# DIRECTORY = "C:\\dev\\maya\\msPlugins\\src\\nodes\\math\\power\\scripts\\templates"
# names = ["Power", "SquareRoot"]

# DIRECTORY = "C:\\dev\\maya\\msPlugins\\src\\nodes\\math\\quaternion\\scripts\\templates"
# names = ["AverageQuaternion", "ConvertQuaternionToEuler", "ConvertQuaternionToMatrix", "MultiplyQuaternion", "MultiplyQuaternionArray", "WeightedAverageQuaternion"]

# DIRECTORY = "C:\\dev\\maya\\msPlugins\\src\\nodes\\math\\range\\scripts\\templates"
# names = ["Clamp", "ClampAngle", "ClampInt", "Remap", "RemapAngle", "RemapInt"]

# DIRECTORY = "C:\\dev\\maya\\msPlugins\\src\\nodes\\math\\round\\scripts\\templates"
# names = ["Ceil", "CeilAngle", "Floor", "FloorAngle", "Round", "RoundAngle"]

# DIRECTORY = "C:\\dev\\maya\\msPlugins\\src\\nodes\\math\\trigonometry\\scripts\\templates"
# names = ["Acos", "Asin", "Atan", "Atan2", "Cos", "Sin", "Tan"]

DIRECTORY = "C:\\dev\\maya\\msPlugins\\src\\nodes\\math\\vector\\scripts\\templates"
names = ["AddVector", "AverageVector", "ClosestPointOnLine", "ClosestPointOnPlane", "CrossProduct", "DivideVector", "DotProduct", "LimitVector", "MultiplyPointByMatrix", "MultiplyVector", "MultiplyVectorByMatrix", "NegateVector", "NormalizeVector", "ReflectVector", "RotateVectorByAxisAngle", "RotateVectorByEuler", "RotateVectorByQuaternion", "SubtractVector", "SumVector", "VectorLength", "VectorLengthSquared", "VectorPlaneProjection", "VectorProjection", "WeightedAverageVector"]


def main():
    # The output string is to be used by the build system
    output = ""

    for name in names:
        filename = "AE" + name + "Template.mel.in"
        filepath = os.path.join(DIRECTORY, filename)
        if not os.path.exists(filepath):
            file = open(filepath, 'w')
            file.close()
            output += "configure_file(\"${PROJECT_DIR}/scripts/templates/" + filename + "\" \"${DIST_INSTALL_DIR}/module/templates/ae/AE${NODE_NAME_PREFIX}" + name + "Template.mel\")\n"

    print output


if __name__ == '__main__':
    main()
