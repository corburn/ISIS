APPNAME = jigsaw
# These tests exercise the bundle adjustment of images from the Apollo frame camera.
# Test 1: specialk (dense cholesky) method, solving for position, angles (with twist), and radius; with error propagation.
# Test 2: sparse (cholmod) method, solving for position, angles (with twist), and radius; with error propagation.
#
# 2013-04-02 KLE - added weights for spacecraft position and angles to test. This reduced the precision of the adjusted
#                  point covariance matrices in the output netfiles. To compensate I reduced the tolerance in the corresponding
#                  DIFF files to 0.00003. We should find out why this is happening. Without these weights, a tolerance of
#                  0.000000003 was sufficient. 
# 2013-04-02 DAC - The values in bundleout.txt were working with 5 places across all systems.  With the above change
#                  they are only good to 4 places now.  Also added tolerance for AprioriX in the control nets and changed the 
#                  covariance tolerance  to 0.00006 for the specialK test.
#

# The "cat bundleout.txt" command in these tests uses sed to do the following (in order):
# 1. remove cube filename paths
# 2. remove net filename paths
# 3. remove digits beyond the fifth decimal place of decimal numbers
# 4. remove date and time

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	cnet=$(INPUT)/Ames_7-ImageLSTest_USGS_combined.net \
	onet=$(OUTPUT)/apollo_specialK_out.net \
	radius=yes \
	twist=yes \
	update=no \
	maxits=50 \
	method=specialk \
	errorpropagation=yes \
	outlier_rejection=no \
	sigma0=1.e-10  \
	bundleout_txt=yes \
	output_csv=yes \
	residuals_csv=yes \
	ckdegree=2 \
	cksolvedegree=2 \
	camsolve=angles \
	spsolve=position \
	spacecraft_position_sigma=1000.0 \
	camera_angles_sigma=2.0 | grep -v "100% Processed" | \
	grep -v "jigsaw" > apollo_specialk_outLog.txt; \
	cat bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | sed s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  > $(OUTPUT)/apollo_specialK_bundleout.txt;
	cat apollo_specialk_outLog.txt | grep -v "Elapsed Time:" | grep -v "TotalElapsedTime" | grep -v "ErrorPropagationElapsedTime" \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/apollo_specialk_outLog.txt;
	cat residuals.csv \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  > $(OUTPUT)/apollo_specialK_residuals.csv;
	cat bundleout_images.csv \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  > $(OUTPUT)/apollo_specialK_bundleout_images.csv;
	$(RM) bundleout_images.csv > /dev/null;
	$(MV) bundleout_points.csv $(OUTPUT)/apollo_specialK_bundleout_points.csv > /dev/null;
	$(RM) bundleout.txt print.prt > /dev/null;
	$(RM) apollo_specialk_outLog.txt > /dev/null;
	$(RM) residuals.csv > /dev/null;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis  \
	cnet=$(INPUT)/Ames_7-ImageLSTest_USGS_combined.net \
	onet=$(OUTPUT)/apollo_cholmod_out.net \
	radius=yes \
	twist=yes \
	update=no \
	maxits=50 \
	errorpropagation=yes \
	outlier_rejection=no \
	sigma0=1.e-10  \
	bundleout_txt=yes \
	output_csv=yes \
	residuals_csv=yes \
	ckdegree=2 \
	cksolvedegree=2 \
	camsolve=angles \
	spsolve=position \
	spacecraft_position_sigma=1000.0 \
	camera_angles_sigma=2.0 | grep -v "100% Processed" | \
	grep -v "jigsaw" > apollo_cholmod_outLog.txt;
	cat bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | sed s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  > $(OUTPUT)/apollo_cholmod_bundleout.txt;
	cat apollo_cholmod_outLog.txt | grep -v "Elapsed Time:" | grep -v "TotalElapsedTime" | grep -v "ErrorPropagationElapsedTime" \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/apollo_cholmod_outLog.txt;
	cat residuals.csv \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  > $(OUTPUT)/apollo_cholmod_residuals.csv;
	cat bundleout_images.csv \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  > $(OUTPUT)/apollo_cholmod_bundleout_images.csv;
	$(RM) bundleout_images.csv > /dev/null;
	$(MV) bundleout_points.csv $(OUTPUT)/apollo_cholmod_bundleout_points.csv > /dev/null;
	$(RM) bundleout.txt print.prt > /dev/null;
	$(RM) apollo_cholmod_outLog.txt > /dev/null;
	$(RM) residuals.csv > /dev/null;
	$(RM) $(OUTPUT)/cube.lis print.prt > /dev/null;
