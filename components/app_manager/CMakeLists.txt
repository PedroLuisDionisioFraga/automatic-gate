idf_component_register(SRCS "app_manager.c"
                    INCLUDE_DIRS "include" "../../secrets"
                    PRIV_REQUIRES wifi_api mqtt5_api gate)
