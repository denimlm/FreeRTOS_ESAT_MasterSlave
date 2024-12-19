COVINFO         :=  $(abspath $(SCRATCH_DIR)/..)/$(EXEC_PREFIX).info
COVINFO_INITIAL :=  $(SCRATCH_DIR)/$(EXEC_PREFIX)_initial.info
LCOV_LIST       :=  $(addsuffix .info,$(addprefix $(SCRATCH_DIR)/,$(SUITE_UT_SRC:.c=)))
PROJ_GCDA_LIST  :=  $(PROJ_SRC_LIST:.c=.gcda)
COV_REPORT_DIR  :=  $(SCRATCH_DIR)/coverage
COVINFO_COMBINE :=  $(SCRATCH_DIR)/$(EXEC_PREFIX)_combined.info


GCOV_OPTS       :=  --unconditional-branches --branch-probabilities

# Cases that run test binaries cannot be run in parallel.
.NOTPARALLEL : $(COVINFO) $(LCOV_LIST) $(PROJ_GCDA_LIST)

# Prevent deletion of intermediate files
NO_DELETE : $(MOCK_HDR_LIST) $(MOCK_SRC_LIST) $(MOCK_OBJ_LIST)      \
            $(DEPS_OBJ_LIST) $(SF_OBJ_LIST) $(EXEC_LIST)            \
            $(PROJ_PP_LIST) $(PROJ_OBJ_LIST) $(PROJ_GCDA_LIST)      \
            $(SUITE_OBJ_LIST) $(RUNNER_SRC_LIST) $(RUNNER_OBJ_LIST) \
            $(COVINFO) $(LCOV_LIST)


# Generate gcov files by default
run : gcov

gcov : $(PROJ_GCDA_LIST)

clean :
    rm -rf $(SCRATCH_DIR)
    rm -f $(BIN_DIR)/$(PROJECT)_utest_*
    rm -f $(COVINFO)

libs :
    make -C $(UT_ROOT_DIR) libs

define run-test
$(1)
endef

$(PROJ_GCDA_LIST) : $(EXEC_LIST)
    rm -f $(PROJ_DIR)/*.gcda
    mkdir -p $(BIN_DIR)
    # run each test case
    $(foreach bin,$^,$(call run-test,$(bin)))

# Run and generate lcov
lcov : $(COVINFO)

lcovhtml : $(COVINFO)
    mkdir -p $(COV_REPORT_DIR)
    genhtml $(COVINFO) $(LCOV_OPTS) --output-directory $(COV_REPORT_DIR)

bin: $(EXEC_LIST)

# Run and append to gcov data files

# Generate callgraph for coverage filtering
$(PROJ_DIR)/callgraph.json : $(PROJ_SRC_LIST)
    mkdir -p $(PROJ_DIR)
#python3 $(UT_ROOT_DIR)/tools/callgraph.py --out $@ $^

# Generate baseline inital coverage data from .gcno file
$(COVINFO_INITIAL) : $(EXEC_LIST)
    lcov $(LCOV_OPTS) --capture --initial --directory $(SCRATCH_DIR) -o $@

# Run the test runner and genrate a filtered gcov.json.gz file
$(SCRATCH_DIR)/%_utest.info : $(BIN_DIR)/$(EXEC_PREFIX)_%_utest                \
                              $(PROJ_DIR)/callgraph.json
    # Remove any existing coverage data
    rm -f $(PROJ_DIR)/*.gcda

    # run the testrunner
    $<

    lcov $(LCOV_OPTS) --directory $(SCRATCH_DIR) --capture -o $@
    # Gather coverage into a json.gz file

#gcov $(GCOV_OPTS) $(SCRATCH_DIR)/$*/$(PROJECT).gcda \
#         --json-format --stdout | gzip > $(subst .info,.json.gz,$@)

    # Filter coverage based on tags in unit test file
#    $(TOOLS_DIR)/filtercov.py --in $(subst .info,.json.gz,$@)   \
#                              --map $(PROJ_DIR)/callgraph.json  \
#                              --test $(PROJ_DIR)/$(PROJECT)_utest_$*.c    \
#                              --format lcov                     \
#                              --out $@
    lcov $(LCOV_OPTS) --summary $@

    # Remove temporary files
    rm -f $(subst .info,.json.gz,$@)
    rm -f $(PROJ_GCDA_LIST)

# Combine lcov from each test bin into one lcov info file for the suite
$(COVINFO_COMBINE) : $(LCOV_LIST)
    lcov $(LCOV_OPTS) -o $@ $(foreach cov,$(LCOV_LIST),--add-tracefile $(cov) )

# Add baseline / initial coverage generated by gcc to point out untagged functions
$(COVINFO) : $(COVINFO_COMBINE) $(COVINFO_INITIAL)
    lcov $(LCOV_OPTS) -o $@ --add-tracefile $(COVINFO_INITIAL) --add-tracefile $(COVINFO_COMBINE)

