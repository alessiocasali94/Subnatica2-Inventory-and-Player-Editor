# Sign $<TARGET_FILE:...> with Authenticode when a PFX is available.
function(s2_sign_target target)
    if(NOT MSVC)
        return()
    endif()

    set(_pfx "${S2_SIGN_PFX}")
    if(NOT _pfx)
        set(_pfx "${CMAKE_SOURCE_DIR}/signing/Subnautica2Panel.pfx")
    endif()
    if(NOT EXISTS "${_pfx}")
        message(STATUS "Code signing skipped (no certificate): ${_pfx}")
        message(STATUS "  Run: powershell -ExecutionPolicy Bypass -File scripts/create-signing-cert.ps1")
        return()
    endif()

    set(_kits_hints)
    if(DEFINED ENV{ProgramFiles})
        list(APPEND _kits_hints "$ENV{ProgramFiles}/Windows Kits/10/bin")
    endif()
    if(DEFINED ENV{ProgramFiles\(x86\)})
        list(APPEND _kits_hints "$ENV{ProgramFiles\(x86\)}/Windows Kits/10/bin")
    endif()
    find_program(S2_SIGNTOOL signtool
        HINTS ${_kits_hints}
        PATH_SUFFIXES
            x64/10.0.26100.0/x64
            x64/10.0.22621.0/x64
            x64
    )
    if(NOT S2_SIGNTOOL)
        message(WARNING "signtool not found — install Windows SDK")
        return()
    endif()

    set(_pass "${S2_SIGN_PFX_PASSWORD}")
    if(NOT _pass)
        set(_pass "Subnautica2Trainer")
    endif()

    set(_tsa "http://timestamp.digicert.com")
    if(S2_SIGN_TIMESTAMP_URL)
        set(_tsa "${S2_SIGN_TIMESTAMP_URL}")
    endif()

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND "${S2_SIGNTOOL}" sign
            /f "${_pfx}"
            /p "${_pass}"
            /fd SHA256
            /tr "${_tsa}"
            /td SHA256
            /d "${S2_PRODUCT_NAME}"
            /du "https://github.com/sabnavr"
            "$<TARGET_FILE:${target}>"
        COMMENT "Authenticode signing $<TARGET_FILE_NAME:${target}>"
        VERBATIM
    )
endfunction()
