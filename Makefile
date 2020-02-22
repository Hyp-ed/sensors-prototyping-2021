
# default build configuration
TARGET  := hyped
MAIN    := run/main.cpp
CROSS   := 0
NOLINT  := 0
VERBOSE := 0
RELEASE := 0

# include helper files
HELPER  := utils/build
include $(HELPER)/config.mk
include $(HELPER)/build.mk
include $(HELPER)/libs.mk
include $(HELPER)/test.mk

.DEFAULT_GOAL := default
default: lint $(TARGET) $(GITHOOKS)

lint:
ifeq ($(NOLINT), 0)
  $(Verb) python2.7 utils/Lint/presubmit.py --workspace=src
endif

lintall:
  $(Echo) -e "\nLinting src/"
  $(Verb) -python2.7 utils/Lint/presubmit.py --workspace=src
  $(Echo) -e "\nLinting run/"
  $(Verb) -python2.7 utils/Lint/presubmit.py --workspace=run
  $(Echo) -e "\nLinting test/"
  $(Verb) $(MAKE) -C test lint --no-print-directory

clean-all: cleanlint cleantest clean

clean:
  $(Verb) rm -rf $(OBJS_DIR)/
  $(Verb) rm -f $(TARGET)

cleanlint:
  $(Verb) rm -f .cpplint-cache
  $(Verb) $(MAKE) -C test cleanlint --no-print-directory

define echo_var
  @echo $(1) = $($1)
endef

cleantest:
  cd test && $(MAKE) clean
  $(Verb) rm -f testrunner

.PHONY: doc
doc:
  $(Verb) doxygen Doxyfile

.PHONY: print
print:
  $(call echo_var,$(VARIABLE))

info:
  $(call echo_var,CC)
  $(call echo_var,SRCS)
  $(call echo_var,OBJS)
  $(call echo_var,UNAME)
  $(call echo_var,CFLAGS)

# PHONY to redo even if .ccls file exists
.PHONY: .ccls
.ccls:
  $(Echo) $(CC) > $@
  $(Verb) $(foreach value,$(CFLAGS) ,echo $(value) >> $@;)
  $(Verb) $(foreach value,$(INC_DIR),echo $(value) >> $@;)

-include $(OBJS:.o=.d)
