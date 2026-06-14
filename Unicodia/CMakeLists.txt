cmake_minimum_required(VERSION 3.16)

file(READ "${CMAKE_SOURCE_DIR}/../VERSION" VERSION_STR_RAW)
string(STRIP "${VERSION_STR_RAW}" VERSION_STR)

# Convert "1.2.3.4" → "1,2,3,4" for FILEVERSION
string(REPLACE "." "," VERSION_COMMA "${VERSION_STR}")

project(Unicodia VERSION ${VERSION_STR} LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Widgets Svg SvgWidgets REQUIRED)

add_compile_definitions(QT_STRINGS)

qt_add_executable(Unicodia
    MANUAL_FINALIZATION
    ../Libs/PugiXml/pugiconfig.hpp ../Libs/PugiXml/pugixml.cpp ../Libs/PugiXml/pugixml.hpp
    ../Libs/ixwebsocket/IXCancellationRequest.cpp ../Libs/ixwebsocket/IXCancellationRequest.h
        ../Libs/ixwebsocket/IXDNSLookup.cpp ../Libs/ixwebsocket/IXDNSLookup.h
        ../Libs/ixwebsocket/IXHttp.cpp ../Libs/ixwebsocket/IXHttp.h
        ../Libs/ixwebsocket/IXHttpClient.cpp ../Libs/ixwebsocket/IXHttpClient.h
        ../Libs/ixwebsocket/IXNetSystem.cpp ../Libs/ixwebsocket/IXNetSystem.h
        ../Libs/ixwebsocket/IXSelectInterrupt.cpp ../Libs/ixwebsocket/IXSelectInterrupt.h
        ../Libs/ixwebsocket/IXSelectInterruptEvent.cpp
        ../Libs/ixwebsocket/IXSelectInterruptEvent.h
        ../Libs/ixwebsocket/IXSelectInterruptFactory.cpp
        ../Libs/ixwebsocket/IXSelectInterruptFactory.h ../Libs/ixwebsocket/IXSocket.cpp
        ../Libs/ixwebsocket/IXSocket.h ../Libs/ixwebsocket/IXSocketConnect.cpp
        ../Libs/ixwebsocket/IXSocketConnect.h ../Libs/ixwebsocket/IXSocketFactory.cpp
        ../Libs/ixwebsocket/IXSocketFactory.h ../Libs/ixwebsocket/IXSocketOpenSSL.cpp
        ../Libs/ixwebsocket/IXSocketOpenSSL.h ../Libs/ixwebsocket/IXSocketTLSOptions.cpp
        ../Libs/ixwebsocket/IXSocketTLSOptions.h ../Libs/ixwebsocket/IXStrCaseCompare.cpp
        ../Libs/ixwebsocket/IXStrCaseCompare.h ../Libs/ixwebsocket/IXUrlParser.cpp
        ../Libs/ixwebsocket/IXUrlParser.h ../Libs/ixwebsocket/IXUserAgent.cpp
        ../Libs/ixwebsocket/IXUserAgent.h ../Libs/ixwebsocket/IXWebSocketHttpHeaders.cpp
        ../Libs/ixwebsocket/IXWebSocketHttpHeaders.h
    ../Libs/L10n/LocDic.cpp ../Libs/L10n/LocDic.h ../Libs/L10n/LocFmt.cpp
        ../Libs/L10n/LocFmt.h ../Libs/L10n/LocManager.cpp ../Libs/L10n/LocManager.h
        ../Libs/L10n/LocQt.cpp ../Libs/L10n/LocQt.h
    # Libs: self-made
    ../Libs/SelfMade/c_TableCache.cpp ../Libs/SelfMade/c_TableCache.h
        ../Libs/SelfMade/c_WrapAroundTable.cpp ../Libs/SelfMade/c_WrapAroundTable.h
        ../Libs/SelfMade/i_DarkMode.cpp ../Libs/SelfMade/i_DarkMode.h
        ../Libs/SelfMade/i_MemStream.cpp ../Libs/SelfMade/i_MemStream.h
        ../Libs/SelfMade/u_Version.cpp ../Libs/SelfMade/u_Version.h
    ../Libs/SelfMade/Fonts/MemFont.cpp ../Libs/SelfMade/Fonts/MemFont.h
        ../Libs/SelfMade/Fonts/TempFont.cpp ../Libs/SelfMade/Fonts/TempFont.h
    ../Libs/SelfMade/GitHub/parsers.cpp ../Libs/SelfMade/GitHub/parsers.h
    ../Libs/SelfMade/Mojibake/cpp/auto_casefold.cpp
    ../Libs/SelfMade/Qt/i_OpenSave.cpp ../Libs/SelfMade/Qt/i_OpenSave.h
        ../Libs/SelfMade/Qt/QtMultiRadio.cpp ../Libs/SelfMade/Qt/QtMultiRadio.h
        ../Libs/SelfMade/Qt/RememberWindow.cpp ../Libs/SelfMade/Qt/RememberWindow.h
        ../Libs/SelfMade/Qt/u_OpenSaveStrings.cpp ../Libs/SelfMade/Qt/u_OpenSaveStrings.h
    ../Libs/SelfMade/Strings/u_EgypIndex.cpp ../Libs/SelfMade/Strings/u_EgypIndex.h
        ../Libs/SelfMade/Strings/u_Qstrings.cpp ../Libs/SelfMade/Strings/u_Qstrings.h
        ../Libs/SelfMade/Strings/u_Strings.cpp ../Libs/SelfMade/Strings/u_Strings.h
    # Main's subdirs
    CharPaint/emoji.cpp CharPaint/emoji.h CharPaint/global.h CharPaint/IconEngines.cpp
        CharPaint/IconEngines.h CharPaint/routines.cpp CharPaint/routines.h
        CharPaint/SkinToneQa.cpp CharPaint/SkinToneQa.h
    Main/FmMain.cpp Main/FmMain.h Main/FmMain.ui Main/HintableLabel.cpp Main/HintableLabel.h
        Main/MainGui.cpp Main/MainGui.h Main/MyHttpClient.cpp Main/MyHttpClient.h
        Main/tables.cpp Main/tables.h Main/WiLibCp.cpp Main/WiLibCp.h Main/WiLibCp.ui Main/WiOsStyle.cpp Main/WiOsStyle.h Main/WiOsStyle.ui Main/WiSample.cpp Main/WiSample.h Main/WiSample.ui Main/WiShowcase.cpp Main/WiShowcase.h Main/WiShowcase.ui
    Search/defs.h Search/engine.cpp Search/engine.h Search/nonAscii.cpp Search/nonAscii.h
        Search/request.cpp Search/request.h Search/trie.h Search/uc.cpp Search/uc.h
    Uc/FontDef.h Uc/FontMatch.cpp Uc/FontMatch.h Uc/UcAuto.cpp Uc/UcAutoCount.h
        Uc/UcAutoDefines.h Uc/UcAutoLib.cpp Uc/UcAutoOldComp.cpp Uc/UcAutoScripts.h
        Uc/UcAutoSutton.cpp Uc/UcBlocks.cpp Uc/UcClipboard.cpp Uc/UcClipboard.h
        Uc/UcCommon.h Uc/UcContinents.h Uc/UcCountries.cpp Uc/UcCp.h Uc/UcData.cpp
        Uc/UcData.h Uc/UcDating.cpp Uc/UcDating.h Uc/UcFlags.h Uc/UcFonts.cpp
        Uc/UcOldInput.cpp Uc/UcOldInput.h Uc/UcRecent.h Uc/UcScripts.cpp Uc/UcSkin.h
    # Main
    main.cpp
    c_SearchEdit.cpp c_SearchEdit.h cmdline.cpp cmdline.h d_Config.cpp d_Config.h
    FmDebugPlural.cpp FmDebugPlural.h FmDebugPlural.ui FmFontSupport.cpp FmFontSupport.h
    FmFontSupport.ui FmMessage.cpp FmMessage.h FmPopup.cpp FmPopup.h FmPopupChar.cpp
    FmPopupChar.h FmPopupChar.ui FmTofuStats.cpp FmTofuStats.h FmTofuStats.ui
    FontBase.cpp FontBase.h LocDefs.h LocList.cpp LocList.h main.cpp MyWiki.cpp
    MyWiki.h qa.cpp qa.h Skin.cpp Skin.h u_Popup.cpp u_Popup.h UpdateUrl.h
    Wiki.cpp Wiki.h WikiLink.cpp WikiLink.h
    # Resources
    Resources/res.qrc
    Resources/Scripts/scripts.qrc
    Main/MyHttpDefs.h
)

if (WIN32)
    # Configure resource file
    configure_file(
        "${CMAKE_SOURCE_DIR}/version.rc.in"
        "${CMAKE_BINARY_DIR}/version.rc"
        @ONLY
    )
    enable_language(RC) # Enable resource compiler
    target_sources(Unicodia PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Unicodia-win.rc
        ${CMAKE_BINARY_DIR}/version.rc)
endif()

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/CharPaint)
include_directories(${CMAKE_SOURCE_DIR}/Main)
include_directories(${CMAKE_SOURCE_DIR}/Uc)
include_directories(${CMAKE_SOURCE_DIR}/../Libs)
include_directories(${CMAKE_SOURCE_DIR}/../Libs/L10n)
include_directories(${CMAKE_SOURCE_DIR}/../Libs/PugiXml)
include_directories(${CMAKE_SOURCE_DIR}/../Libs/RapidJson)
include_directories(${CMAKE_SOURCE_DIR}/../Libs/SelfMade)
include_directories(${CMAKE_SOURCE_DIR}/../Libs/SelfMade/L10n)
include_directories(${CMAKE_SOURCE_DIR}/../Libs/SelfMade/Mojibake)
include_directories(${CMAKE_SOURCE_DIR}/../Libs/SelfMade/Qt)
include_directories(${CMAKE_SOURCE_DIR}/../Libs/SelfMade/Strings)
include_directories(${CMAKE_SOURCE_DIR}/../Libs/Zippy)

target_link_libraries(Unicodia PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Unicodia PRIVATE Qt${QT_VERSION_MAJOR}::Svg)
target_link_libraries(Unicodia PRIVATE Qt${QT_VERSION_MAJOR}::SvgWidgets)

if (WIN32)
    target_link_libraries(Unicodia PRIVATE ws2_32)
endif()

set_target_properties(Unicodia PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Unicodia
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Unicodia)
endif()
