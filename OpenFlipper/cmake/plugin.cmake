# This module provides the following macro:
#
# openflipper_plugin ( [DIRS dir1 dir2 ...]
#                      [DEPS dep1 dep2 ...]
#                      [OPTDEPS dep1 dep2 ...]
#                      [LDFLAGSADD flag1 flag2 ...]
#                      [CFLAGSADD flag1 flag2 ...]
#                      [LIBRARIES lib1 lib2 ...]
#                      [LIBDIRS dir1 dir2 ...]
#                      [INCDIRS dir1 dir2 ...]
#                      [ADDSRC file1 file2 ...]
#                      [INSTALLDATA dir1 dir2 ...]
#                      [TRANSLATION_LANGUAGES lang1 lang2 ...]
#                      [TRANSLATION_ADDFILES file1 file2 ...]
#                      [LICENSEMANAGER])
#
# DIRS        = additional directories with source files
# DEPS        = required dependencies for find_package macro
# OPTDEPS     = optional dependencies for find_package macro, if found, a define ENABLE_<Depname> will be added automatically
# LDFLAGSADD  = flags added to the link command
# CFLAGSADD   = flags added to the compile command
# LIBRARIES   = libraries added to link command
# LIBDIRS     = additional link directories
# INCDIRS     = additional include directories
# ADDSRC      = additional source files
# INSTALLDATA = directories that will be installed into the openflipper data directory
#
# TRANSLATION_LANGUAGES = language codes for translation
# TRANSLATION_ADDFILES  = additional files that should be included into the translation files
#
# LICENSEMANAGER = Compile plugin with license management
#
# Plugins can implement a acg_list_filter ("listname") macro to filter out
# unneeded headers, sources and ui files from the autogenerated 
# build sources lists (see common.cmake drop_templates macro)
#
# Translations
# Create a subdirectory called translations in your Plugin directory
# The *.ts files there will be used for the translations
#
# Special DEPS:
# CUDA: 
# If your plugin requires cuda, add "CUDA" to the list of dependencies
# and create a subdir called Cuda in your plugin directory
# The files in this directory will be compiled by nvcc and linked
# into the plugin
#
# The variables PLUGIN_${plugin}_MISSING_DEPS indicate whether any mandatory
# dependencies of the respective plugins are missing.
#

include (ACGCommon)

# get plugin name from directory name
macro (_get_plugin_name var)
  string (REGEX MATCH "Plugin-.+[/\\]?$" _dir ${CMAKE_CURRENT_SOURCE_DIR})
  string (REPLACE "Plugin-" "" ${var} ${_dir})
endmacro ()

# parse plugin macro parameter
macro (_get_plugin_parameters _prefix)
    set (_current_var _foo)
    set (_supported_var DIRS DEPS OPTDEPS LDFLAGSADD CFLAGSADD LIBRARIES LIBDIRS INCDIRS ADDSRC INSTALLDATA TRANSLATION_LANGUAGES TRANSLATION_ADDFILES)
    set (_supported_flags LICENSEMANAGER)
    foreach (_val ${_supported_var})
        set (${_prefix}_${_val})
    endforeach ()
    foreach (_val ${_supported_flags})
        set (${_prefix}_${_val} 0)
    endforeach ()

    foreach (_val ${ARGN})
        set (_found FALSE)
        foreach (_find ${_supported_var})
            if ("${_find}" STREQUAL "${_val}")
                set (_found TRUE)
            endif ()
        endforeach ()

        foreach (_find ${_supported_flags})
            if ("${_find}" STREQUAL "${_val}")
                set (${_prefix}_${_val} 1)
            endif ()
        endforeach ()

        if (_found)
            set (_current_var ${_prefix}_${_val})
        else ()
            list (APPEND ${_current_var} ${_val})
        endif ()
    endforeach ()

endmacro ()

# check dependencies
# _prefix    : prefix used ( usually the plugin name )
# _optional : if we are currently pars
macro (_check_plugin_deps _prefix _optional )

    set (${_prefix}_HAS_DEPS TRUE)

    # This will contain the final list of all dependencies
    list (APPEND FULL_DEPENDENCY_LIST "")

    #======================================================================================       
    # Collect dependencies of dependencies
    # Recursive dependencies will be added here. 
    # The base package name will be used and the variables
    # PACKAGE_DEPS     : Mandatory recursive dependencies
    # PACKAGE_OPT_DEPS : Optional recursive dependencies
    #======================================================================================
    foreach (_val ${ARGN})
       string (TOUPPER ${_val} _VAL)
	
       # First we try to find the dependencies directly
       find_package(${_val})

       # This will contain the list of all dependencies of the current base dependency ( including recursive dependencies for one level )
       set (CURRENT_DEPENDENCY_LIST "")

       # Flag if all dependencies were found (Only mandatory ones) 
       set (ALL_REQUIRED_DEPENDENCIES_FOUND TRUE)

       # Recursive dependencies which are not optional will also be added by default (Not optional, so error if not available!)
       if  ( DEFINED ${_VAL}_DEPS )

          foreach (_rec_dep ${${_VAL}_DEPS})

            string (TOUPPER ${_rec_dep} _REC_DEP)

            find_package(${_rec_dep})
    
            if ( ${_REC_DEP}_FOUND )
              list (APPEND CURRENT_DEPENDENCY_LIST ${_rec_dep} )
              add_definitions( -DENABLE_${_REC_DEP} )
            else()
              set (ALL_REQUIRED_DEPENDENCIES_FOUND FALSE)
              acg_set (_${_prefix}_MISSING_DEPS "${_${_prefix}_MISSING_DEPS} ${_VAL}_MISSING_MANDATORY_RECURSIVE_DEP_${_REC_DEP}")
            endif()
          endforeach()
          
       endif()


       # optional dependencies will be added if available and compiler flags will be set accordingly
       if  ( DEFINED ${_VAL}_OPT_DEPS )

          foreach (_rec_dep ${${_VAL}_OPT_DEPS})

            string (TOUPPER ${_rec_dep} _REC_DEP)

            find_package(${_rec_dep})

            # Optional so add if we found the dependency or we skip it.
            # Defines will be added due to the optional status	
            if ( ${_REC_DEP}_FOUND )
              list (APPEND CURRENT_DEPENDENCY_LIST ${_rec_dep} )
              add_definitions( -DENABLE_${_REC_DEP} )
            else()
              add_definitions( -DDISABLE_${_REC_DEP} )
	    endif()
          endforeach()
       endif()

       if ( ${_optional} STREQUAL "TRUE"  )

         if ( ${_VAL}_FOUND )

           # Optional dependency found and recursive found, so add definition to enable it
           if ( ${_optional} STREQUAL "TRUE" )
             add_definitions(-DENABLE_${_VAL})
           endif()

           if ( ALL_REQUIRED_DEPENDENCIES_FOUND )
             # All found so add package dependencies
             list (APPEND FULL_DEPENDENCY_LIST ${CURRENT_DEPENDENCY_LIST})
             # Add package itself
             list (APPEND FULL_DEPENDENCY_LIST ${_val})
           else()
	     message("Missing recursive dependencies when optional")
           endif()

         else()
           message("Info: OPTIONAL Dependency  ${_VAL} for  ${_prefix}  not found .. proceeding without it!")

         endif()

       else()

         if ( ALL_REQUIRED_DEPENDENCIES_FOUND )
           # All found so add package dependencies
           list (APPEND FULL_DEPENDENCY_LIST ${CURRENT_DEPENDENCY_LIST})
           # Add package itself
           list (APPEND FULL_DEPENDENCY_LIST ${_val})
         else()
    	   acg_set (_${_prefix}_MISSING_DEPS "${_${_prefix}_MISSING_DEPS} ${_VAL}_BECAUSE_OF_MISSING_RECURSIVE_DEPENDENCY")
           set (${_prefix}_HAS_DEPS FALSE)
         endif()

       endif()

    endforeach()

    foreach (_val ${FULL_DEPENDENCY_LIST})
        string (TOUPPER ${_val} _VAL)

        find_package(${_val})

        #======================================================================================
        # Global dependency tracking
        #======================================================================================
        # Get our current list of all dependencies used by plugins
	get_property( global_dependency_list GLOBAL PROPERTY GLOBAL_PLUGIN_DEPENDENCIES_LIST)

	#Add the new entry to the list
        list (APPEND global_dependency_list ${_val} )

        # Remove it again, if it was a duplicate
        list(REMOVE_DUPLICATES global_dependency_list)
             
        # Store the list in the global property
        set_property( GLOBAL PROPERTY GLOBAL_PLUGIN_DEPENDENCIES_LIST ${global_dependency_list} )

        #======================================================================================
        # Collect all dependencies (local and global)
        #======================================================================================

        if (${_val}_FOUND OR ${_VAL}_FOUND)
          foreach (_name ${_val} ${_VAL})
            if (DEFINED ${_name}_INCLUDE_PATH)
              list (APPEND ${_prefix}_DEPS_INCDIRS "${${_name}_INCLUDE_PATH}")
            endif ()
            if (DEFINED ${_name}_INCLUDE_DIRS)
              list (APPEND ${_prefix}_DEPS_INCDIRS "${${_name}_INCLUDE_DIRS}")
            endif ()
            if (DEFINED ${_name}_INCLUDE_DIR)
              list (APPEND ${_prefix}_DEPS_INCDIRS "${${_name}_INCLUDE_DIR}")
            endif ()
            if (DEFINED ${_name}_LIBRARY_DIRS)
              list (APPEND ${_prefix}_DEPS_LIBDIRS "${${_name}_LIBRARY_DIRS}")
            endif ()
            if (DEFINED ${_name}_LIBRARY_DIR)
              list (APPEND ${_prefix}_DEPS_LIBDIRS "${${_name}_LIBRARY_DIR}")
            endif ()
            if (DEFINED ${_name}_LIBRARIES)
              list (APPEND ${_prefix}_DEPS_LIBRARIES "${${_name}_LIBRARIES}")
            endif ()
            if (DEFINED ${_name}_LIBRARY)
              list (APPEND ${_prefix}_DEPS_LIBRARIES "${${_name}_LIBRARY}")
            endif ()
            if (DEFINED ${_name}_LINKER_FLAGS)
              list (APPEND ${_prefix}_DEPS_LINKER_FLAGS "${${_name}_LINKER_FLAGS}")
            endif ()
          endforeach ()
        else ()
            set (${_prefix}_HAS_DEPS FALSE)
            acg_set (_${_prefix}_MISSING_DEPS "${_${_prefix}_MISSING_DEPS} ${_val}")
        endif ()
    endforeach ()
endmacro ()

macro (_plugin_licensemanagement)
  acg_append_files (headers "LicenseManager.hh" "${CMAKE_SOURCE_DIR}/OpenFlipper/LicenseManager")

  if ( OPENFLIPPER_ENABLE_LICENSE_MANAGER )
    acg_append_files (headers "LicenseManagerActive.hh" "${CMAKE_SOURCE_DIR}/OpenFlipper/LicenseManager")
    acg_append_files (sources "*.cc" "${CMAKE_SOURCE_DIR}/OpenFlipper/LicenseManager")
  # acg_append_files (ui "*.ui" "${CMAKE_SOURCE_DIR}/OpenFlipper/LicenseManager")

    acg_append_files (keygen_hdr "*.hh" "${CMAKE_SOURCE_DIR}/OpenFlipper/LicenseManager/keyGen")
    acg_append_files (keygen_src "*.cc" "${CMAKE_SOURCE_DIR}/OpenFlipper/LicenseManager/keyGen")
    acg_append_files (keygen_ui  "*.ui" "${CMAKE_SOURCE_DIR}/OpenFlipper/LicenseManager/keyGen")
  
    # genereate uic and moc targets
    acg_qt4_autouic (keygen_uic ${keygen_ui})
    acg_qt4_automoc (keygen_moc ${keygen_hdr})

    add_executable (Plugin-${plugin}-keygen ${keygen_uic} ${keygen_moc} ${keygen_hdr} ${keygen_src})

    target_link_libraries (
      Plugin-${plugin}-keygen
      ${QT_LIBRARIES}
    )

    # create our output directroy
    if (NOT EXISTS ${CMAKE_BINARY_DIR}/LicenseManagement)
      file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/LicenseManagement)
    endif ()

    if (WIN32)
      # copy exe file to "Build" directory
      # Visual studio will create this file in a subdirectory so we can't use
      # RUNTIME_OUTPUT_DIRECTORY directly here
      add_custom_command (TARGET Plugin-${plugin}-keygen POST_BUILD
                          COMMAND ${CMAKE_COMMAND} -E
                          copy_if_different
                            ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Plugin-${plugin}-keygen.exe
                          ${CMAKE_BINARY_DIR}/LicenseManagement/Plugin-${plugin}-keygen.exe)
    else ()
      set_target_properties (
        Plugin-${plugin}-keygen PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/LicenseManagement"
      )
    endif ()
  else(OPENFLIPPER_ENABLE_LICENSE_MANAGER)
    acg_append_files (headers "LicenseManagerDisabled.hh" "${CMAKE_SOURCE_DIR}/OpenFlipper/LicenseManager")
  endif()
endmacro ()

# main function
function (_build_openflipper_plugin plugin)

  acg_set (OPENFLIPPER_${_PLUGIN}_BUILD "0")

  # get upper plugin name
  string (TOUPPER ${plugin} _PLUGIN)

  #============================================================================================
  # parse parameters
  #============================================================================================
  
  _get_plugin_parameters (${_PLUGIN} ${ARGN})

  #============================================================================================
  # Collect dependencies
  #============================================================================================

  # check dependencies
  acg_unset (_${_PLUGIN}_MISSING_DEPS)
  set (${_PLUGIN}_HAS_DEPS)
  _check_plugin_deps (${_PLUGIN} TRUE ${${_PLUGIN}_OPTDEPS})

  acg_unset (_${_PLUGIN}_MISSING_DEPS)
  set (${_PLUGIN}_HAS_DEPS)
  _check_plugin_deps (${_PLUGIN} FALSE ${${_PLUGIN}_DEPS})

  #============================================================================================
  # Remember Lib dirs for bundle generation
  #============================================================================================

  # On Windows we collect the library paths in a global property.
  # These paths will be used in the toplevel cmakefile
  # to configure the fixup_bundle to copy the required libs to the bundle.
  if (WIN32)
    set(WINDOWS_COPY_LIBDIRS "")
    # Get the required directories and remeber them to build the bundle later
    list(APPEND WINDOWS_COPY_LIBDIRS  ${${_PLUGIN}_DEPS_LIBDIRS} )
    # Remove duplicates from the list
    list(REMOVE_DUPLICATES WINDOWS_COPY_LIBDIRS)
    # Has to be a global property because we need it in the main cmakefile
    set_property(GLOBAL APPEND PROPERTY WINDOWS_LIBRARY_DIR_LIST "${WINDOWS_COPY_LIBDIRS}")
  endif(WIN32)

  if (${_PLUGIN}_HAS_DEPS)
    include_directories (
      .
      ${CMAKE_SOURCE_DIR}
      ${OPENMESH_INCLUDE_DIR}
      ${OPENVOLUMEMESH_INCLUDE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
      ${${_PLUGIN}_DEPS_INCDIRS}
      ${${_PLUGIN}_INCDIRS}
      ${OPENGL_INCLUDE_DIR}
      ${GLEW_INCLUDE_DIR}
      ${GLUT_INCLUDE_DIR}
      ${CMAKE_BINARY_DIR}/OpenFlipper/PluginLib
    )

    # Linking for apple is special here as the linker pulls in the dependencies, we have to set them like in PluginLib!
    if( APPLE )

      # search all ObjectTypes in the ObjectType directory for additional build information
      file (
         GLOB _plugin_buildinfos
         RELATIVE "${CMAKE_SOURCE_DIR}"
         "${CMAKE_SOURCE_DIR}/ObjectTypes/*/CMakeLists.txt"
      )

   
      # include all cmake files found for objecttypes here
      foreach ( _buildInfo ${_plugin_buildinfos})
        include ("${CMAKE_SOURCE_DIR}/${_buildInfo}")
      endforeach ()

      link_directories (
        ${${_PLUGIN}_DEPS_LIBDIRS}
        ${${_PLUGIN}_LIBDIRS}
        ${ADDITIONAL_PLUGINLIB_LINK_DIRS} 
      )
   
    else (APPLE)
       link_directories (
         ${${_PLUGIN}_DEPS_LIBDIRS}
         ${${_PLUGIN}_LIBDIRS}
       )
    endif(APPLE)
 
    set (directories 
       . 
       ${${_PLUGIN}_DIRS}
    )

    #============================================================================================
    # User Documentation build
    #============================================================================================
    
    # Only build the documentation if a userdoc subdirectory exists for the plugin
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/userDoc)

      set(plugin_html_doc_dir "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Doc/UserHTML/Plugin-${plugin}")
      set(plugin_qt_help_dir "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/Help")
    
      find_package(Doxygen)

      if ( DOXYGEN_FOUND )

        # Create user documentation target with our template doxy file 
        acg_create_doc_target( doc-User-${plugin} "${CMAKE_SOURCE_DIR}/OpenFlipper/Documentation/PluginDoxyTemplate" )

        # make doc builds this plugin's documentation as well
        add_dependencies(doc-UserHelp doc-User-${plugin})

        # Create directories in order to avoid doxygen warnings
        if(NOT (IS_DIRECTORY ${plugin_html_doc_dir}) )
           file(MAKE_DIRECTORY ${plugin_html_doc_dir} )
        endif()

        # Create directories in order to avoid doxygen warnings
        if(NOT (IS_DIRECTORY ${plugin_qt_help_dir}) )
           file(MAKE_DIRECTORY ${plugin_qt_help_dir} )
        endif()                                                

      endif()

    endif()

    #============================================================================================
    # Binary build
    #============================================================================================


    # collect all header,source and ui files
    acg_append_files (headers "*.hh" ${directories})
    acg_append_files (sources "*.cc" ${directories})
    acg_append_files (sources "*.cpp" ${directories})
    acg_append_files (sources "*.c" ${directories})
    acg_append_files (ui "*.ui" ${directories})
    acg_append_files (qrc "*.qrc" ${directories})

    # remove template cc files from source file list
    acg_drop_templates (sources)

    # filter header,source and ui files lists if acg_list_filter macro has been defined
    if (COMMAND acg_list_filter)
      acg_list_filter (headers)
      acg_list_filter (sources)
      acg_list_filter (ui)
    endif ()

    set(${_PLUGIN}_LICENSE_DEFS "")
    if (${_PLUGIN}_LICENSEMANAGER )
      _plugin_licensemanagement ()
 
      if ( OPENFLIPPER_ENABLE_LICENSE_MANAGER )
	   set(${_PLUGIN}_LICENSE_DEFS "-DWITH_LICENSE_MANAGER")
      endif()
    endif ()

    # genereate uic and moc targets
    acg_qt4_autouic (uic_targets ${ui})
    acg_qt4_automoc (moc_targets ${headers})
    acg_qt4_autoqrc (qrc_targets ${qrc})
    
    
    
    add_library (Plugin-${plugin} MODULE ${uic_targets} ${sources} ${headers} ${moc_targets} ${qrc_targets} ${${_PLUGIN}_ADDSRC})

    # add this plugin to build plugin list for dependency tracking
    acg_set (OPENFLIPPER_PLUGINS "${OPENFLIPPER_PLUGINS};Plugin-${plugin}")
    acg_set (OPENFLIPPER_${_PLUGIN}_BUILD "1")

    # append compiler and linker flags from plugin dependencies
    set_target_properties (
      Plugin-${plugin} PROPERTIES
      COMPILE_FLAGS "${${_PLUGIN}_CFLAGSADD} ${${_PLUGIN}_LICENSE_DEFS}"
      LINK_FLAGS "${${_PLUGIN}_LDFLAGSADD} ${${_PLUGIN}_DEPS_LINKER_FLAGS}"
    )
    
    if (WIN32)
      # Visual studio requires our plugins to link with GLUT
      find_package (GLUT)
      # generate dllinport defines
      add_definitions (-DACGDLL -DUSEACG -DPLUGINLIBDLL -DUSEPLUGINLIBDLL)
      target_link_libraries (Plugin-${plugin}
    	OpenMeshCore
        OpenMeshTools
        ${OPENVOLUMEMESH_LIBRARY}
        ACG
        OpenFlipperPluginLib
        ${GLUT_LIBRARIES}
      )

      # copy plugin dll file to "Build" directory
      # Visual studio will create this file in a subdirectory so we can't use
      # LIBRARY_OUTPUT_DIRECTORY directly here
      if (NOT EXISTS ${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_PLUGINDIR})
        file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Build/${ACG_PROJEC_PLUGINDIR})
      endif ()
      add_custom_command (TARGET Plugin-${plugin} POST_BUILD
                          COMMAND ${CMAKE_COMMAND} -E
                          copy_if_different
                            ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Plugin-${plugin}.dll
                            ${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_PLUGINDIR}/Plugin-${plugin}.dll
                          )
    elseif (APPLE) 

      target_link_libraries (
         Plugin-${plugin}
	 OpenFlipperPluginLib
	 ${QT_LIBRARIES}
      )
      # copy plugin so file to application bundle inside "Build" directory
      if (NOT EXISTS ${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_PLUGINDIR})
        file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_PLUGINDIR})
      endif ()
      add_custom_command (TARGET Plugin-${plugin} POST_BUILD
                          COMMAND ${CMAKE_COMMAND} -E
                          copy_if_different
                            ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/libPlugin-${plugin}.so
                            ${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_PLUGINDIR}/libPlugin-${plugin}.so
                          )
    else ()
      # directly generate plugin in plugin directory
      set_target_properties (
        Plugin-${plugin} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_PLUGINDIR}"
      )
    endif ()
    
    target_link_libraries (Plugin-${plugin}
      ${${_PLUGIN}_DEPS_LIBRARIES}
      ${${_PLUGIN}_LIBRARIES}
    )

    # no install on mac, because the whole bundle will be installed in the
    # toplevel CMakeLists.txt
    if (NOT APPLE)
      install (
        TARGETS Plugin-${plugin}
        DESTINATION ${ACG_PROJECT_PLUGINDIR}
      )
    endif ()

    foreach (_dir ${${_PLUGIN}_INSTALLDATA})
      acg_copy_after_build (Plugin-${plugin} "${CMAKE_CURRENT_SOURCE_DIR}/${_dir}" "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_DATADIR}/${_dir}")
      if (NOT APPLE)
        acg_install_dir ("${CMAKE_CURRENT_SOURCE_DIR}/${_dir}" "${ACG_PROJECT_DATADIR}/${_dir}")
      endif ()
    endforeach ()
    
    #============================================================================================
    # CUDA Integration
    # This block takes all cuda files from a plugins subdirectory called "Cuda"
    # The files will be compiled by nvcc and linked to one library containing all cuda parts of 
    # the plugin. This static library is than linked into the plugin
    #============================================================================================
    
    # Check if cuda is in the list of dependencies:
    list(FIND ${_PLUGIN}_DEPS "CUDA" FIND_RESULT)
    if ( NOT FIND_RESULT EQUAL -1 )
    
      # Required but maybe it is not found
      # finder is automatically called before this
      if ( CUDA_FOUND )
        #Setup options
        CUDA_ADD_CUDA_INCLUDE_ONCE()
 
        # Clear out the directory defines to prevent nvcc from getting them
        # TODO : Create an additional flag variable for CUDA
        get_directory_property( CURRENT_DEFINITIONS COMPILE_DEFINITIONS )
        set_directory_properties( PROPERTIES COMPILE_DEFINITIONS "" )

        get_directory_property( CURRENT_DEFINITIONS_A COMPILE_DEFINITIONS )

        # Get all cuda sources
        file(GLOB_RECURSE ${_PLUGIN}_CUDA_SRCS Cuda/*.cu Cuda/*.c)
        
        # Add cuda include dirs to global build target
        include_directories(${CUDA_INCLUDE_DIRS})
        
        # Force to -fPIC such that we can correctly link against the lib lateron in the plugin
        list ( APPEND CMAKE_CXX_FLAGS "-fPIC" )
        list ( APPEND CMAKE_CC_FLAGS "-fPIC" )
 
        # Prepare the files
        if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        	CUDA_COMPILE(${_PLUGIN}_CUDA_GENERATED_FILES ${${_PLUGIN}_CUDA_SRCS} OPTIONS -g -G --device-emulation -Xcompiler)
        else()
        	CUDA_COMPILE(${_PLUGIN}_CUDA_GENERATED_FILES ${${_PLUGIN}_CUDA_SRCS} OPTIONS -O2)
        endif()
       
        # Create the library containing all cuda files
        CUDA_ADD_LIBRARY(${_PLUGIN}_cuda_lib STATIC ${${_PLUGIN}_CUDA_GENERATED_FILES} )
       
        # Link the cuda plugin library to the plugin itself
        target_link_libraries(Plugin-${plugin} ${_PLUGIN}_cuda_lib)
       
        # reset to original compile definitions
        set_directory_properties( PROPERTIES COMPILE_DEFINITIONS "${CURRENT_DEFINITIONS}" )

      endif( CUDA_FOUND )
    endif()

    #============================================================================================
    # Translations
    #============================================================================================

    if ( ${_PLUGIN}_TRANSLATION_LANGUAGES)
      set (trans_files ${sources} )
      list (APPEND trans_files ${ui})
      list (APPEND trans_files ${headers})
      list (APPEND trans_files ${${_PLUGIN}_TRANSLATION_ADDFILES})
      acg_add_translations(Plugin-${plugin} ${${_PLUGIN}_TRANSLATION_LANGUAGES} "${trans_files}" )
    endif ()
    set(PLUGIN_${plugin}_MISSING_DEPS FALSE PARENT_SCOPE)

  else ()
    message (STATUS "[WARNING] One or more dependencies for plugin ${plugin} not found. Skipping plugin.")
    message (STATUS "Missing dependencies :${_${_PLUGIN}_MISSING_DEPS}")
    set(PLUGIN_${plugin}_MISSING_DEPS TRUE PARENT_SCOPE)
  endif ()
  
endfunction ()

macro (openflipper_plugin)
  _get_plugin_name (_plugin)

  string (TOUPPER ${_plugin} _PLUGIN)

  # add option to disable plugin build
  option (
    DISABLE_PLUGIN_${_PLUGIN}
    "Disable building of plugin \"${_plugin}\""
        OFF
  )

  if (NOT DISABLE_PLUGIN_${_PLUGIN})
    _build_openflipper_plugin (${_plugin} ${ARGN})
  endif ()
endmacro ()
