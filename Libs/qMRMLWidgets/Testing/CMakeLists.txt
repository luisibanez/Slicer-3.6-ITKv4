
SET(KIT qMRMLWidgets)
SET(CMAKE_TESTDRIVER_AFTER_TESTMAIN  "DEBUG_LEAKS_ENABLE_EXIT_ERROR();")
CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  qMRMLColorTableComboBoxTest1.cxx
  qMRMLItemHelperTest1.cxx
  qMRMLLinearTransformSliderTest1.cxx
  qMRMLListWidgetTest1.cxx
  qMRMLMatrixWidgetTest1.cxx
  qMRMLModelTest1.cxx
  qMRMLLabelComboBoxTest1.cxx
  qMRMLNodeComboBoxTest1.cxx
  qMRMLNodeComboBoxTest2.cxx
  qMRMLNodeComboBoxTest3.cxx
  qMRMLNodeComboBoxTest4.cxx
  qMRMLNodeFactoryTest1.cxx
  qMRMLSceneFactoryWidgetTest1.cxx
  qMRMLSceneModelTest1.cxx
  #qMRMLTransformProxyModelTest1.cxx
  qMRMLSceneTransformModelTest1.cxx
  qMRMLSceneTransformModelTest2.cxx
  qMRMLTransformSlidersTest1.cxx
  qMRMLThreeDViewTest1.cxx
  qMRMLTreeWidgetTest1.cxx
  qMRMLUtilsTest1.cxx
  qMRMLWidgetsExportTest1.cxx
  EXTRA_INCLUDE TestingMacros.h
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CxxTests.cxx)

SET(LIBRARY_NAME ${PROJECT_NAME})

ADD_EXECUTABLE(${KIT}CxxTests ${Tests})
TARGET_LINK_LIBRARIES(${KIT}CxxTests ${LIBRARY_NAME})

SET( qMRMLWidgets_TESTS ${CXX_TEST_PATH}/${KIT}CxxTests)
IF(WIN32)
  SET(qMRMLWidgets_TESTS ${CXX_TEST_PATH}/${CMAKE_BUILD_TYPE}/${KIT}CxxTests)
ENDIF(WIN32)

MACRO( SIMPLE_TEST  TESTNAME )
  ADD_TEST( ${TESTNAME} ${LAUNCH_EXE} ${qMRMLWidgets_TESTS} ${TESTNAME} )
ENDMACRO( SIMPLE_TEST  )

MACRO( SCENE_TEST  TEST SCENENAME )
ADD_TEST( ${TEST}_${SCENENAME} 
  ${LAUNCH_EXE}  ${qMRMLWidgets_TESTS} ${TEST}
  ${MRML_SOURCE_DIR}/Testing/${SCENENAME}
  )
ENDMACRO( SCENE_TEST )

SIMPLE_TEST( qMRMLColorTableComboBoxTest1 )
SIMPLE_TEST( qMRMLItemHelperTest1 )
SIMPLE_TEST( qMRMLLinearTransformSliderTest1 )
SIMPLE_TEST( qMRMLListWidgetTest1 )
SIMPLE_TEST( qMRMLMatrixWidgetTest1 )
SIMPLE_TEST( qMRMLModelTest1 )
SIMPLE_TEST( qMRMLLabelComboBoxTest1 )
SIMPLE_TEST( qMRMLNodeComboBoxTest1 )
SIMPLE_TEST( qMRMLNodeComboBoxTest2 )
SCENE_TEST ( qMRMLNodeComboBoxTest3 diffusionEditor.mrml)
SIMPLE_TEST ( qMRMLNodeComboBoxTest4)
SIMPLE_TEST( qMRMLNodeFactoryTest1 )
SIMPLE_TEST( qMRMLSceneFactoryWidgetTest1 )
SIMPLE_TEST( qMRMLSceneModelTest1 )
SIMPLE_TEST( qMRMLSceneTransformModelTest1 )
SCENE_TEST( qMRMLSceneTransformModelTest2 vol_and_cube.mrml)
SIMPLE_TEST( qMRMLTransformSlidersTest1 )
SIMPLE_TEST( qMRMLThreeDViewTest1)
SCENE_TEST( qMRMLTreeWidgetTest1 vol_and_cube.mrml)
SIMPLE_TEST( qMRMLUtilsTest1 )
SIMPLE_TEST( qMRMLWidgetsExportTest1 )
