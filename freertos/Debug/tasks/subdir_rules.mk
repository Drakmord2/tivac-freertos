################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
tasks/%.obj: ../tasks/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.3.LTS/include" --include_path="/Users/Drak/Projects/tivac-freertos/freertos" --include_path="/Users/Drak/ti/tivaware_c_series_2_1_4_178/examples/boards/ek-tm4c123gxl" --include_path="/Users/Drak/ti/tivaware_c_series_2_1_4_178" --include_path="/Users/Drak/ti/tivaware_c_series_2_1_4_178/third_party" --include_path="/Users/Drak/ti/tivaware_c_series_2_1_4_178/third_party/FreeRTOS/Source/include" --include_path="/Users/Drak/ti/tivaware_c_series_2_1_4_178/third_party/FreeRTOS" --include_path="/Users/Drak/ti/tivaware_c_series_2_1_4_178/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power=all -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="tasks/$(basename $(<F)).d_raw" --obj_directory="tasks" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


