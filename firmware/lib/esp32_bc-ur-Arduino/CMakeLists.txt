idf_component_register(SRC_DIRS "src"
        PRIV_REQUIRES mbedtls cbor
        INCLUDE_DIRS "src")

target_compile_features(${COMPONENT_LIB} PRIVATE cxx_std_20)
target_compile_options(${COMPONENT_LIB} PRIVATE -ffast-math)
