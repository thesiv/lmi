# This demonstration shows a way to source a script in a makefile,
# and export environment variables set by that script to make's
# environment, thus:
#  - add an 'env.make' prerequisite for the top-level makefile,
#    with recipe "$(eval include env.make)"
#  - add a phony target for 'env.make' that sources the script,
#    then writes 'make' assignments like "export foo := bar"
#    for each desired environment variable
# To test:
#   $export LMI_IN=X; make -f parent.make all
#   $export LMI_IN=Q; make -f parent.make all
# and check what appears on stdout.

# A file named 'env.make' is created in the source directory.
# That unwanted side effect can be avoided by replacing it
# with, say, '/run/var/lmi/env.make'.

# lmi's 'GNUmakefile' uses a double-colon rule like this:
#parent.make $(srcdir)/parent.make:: ;
# which becomes relevant below. Possible problem: adding a
# prerequisite may impair the efficiency that was gained by
# writing the double-colon rule.

# No good: syntax error.
#include set.sh

# Neither necessary nor sufficient.
#include env.make

# This can't coexist with the desired double-colon rule (see above):
#parent.make: env.make
parent.make parent.make:: env.make ;
# $(eval include) here is necessary and sufficient.
	$(eval include env.make)
	@echo "eval: LMI_IN in 'parent.make': $$LMI_IN"
	@echo "eval: LMI_OUT in 'parent.make': $$LMI_OUT"

# Doesn't execute recipe without ".PHONY":
.PHONY: env.make
# This prerequisite is unnecessary:
#env.make: set.sh
env.make:
	@echo "Sourcing 'set.sh'"
	. ./set.sh ; echo "export LMI_OUT := $$LMI_OUT" > env.make
	@echo "target: LMI_IN in 'parent.make': $$LMI_IN"
	@echo "target: LMI_OUT in 'parent.make': $$LMI_OUT ...but wait..."

all:
	@echo "LMI_OUT for targets in 'parent.make': $$LMI_OUT"
	$(MAKE) --no-print-directory -f child.make

# Obviously one could simply write a cover script to replace direct
# invocation of 'make', but that's nasty. See:
#   https://lists.gnu.org/archive/html/help-make/2006-04/msg00142.html
# which suggests 'eval'.
#
# This is unhelpful:
#   https://stackoverflow.com/questions/7507810/how-to-source-a-script-in-a-makefile
# except that it has a link to:
#   https://blog.153.io/2016/04/18/source-a-shell-script-in-make/
# which doesn't actually work in its final, simplified version;
# but earlier in that article it uses 'eval include', which does work.
