INCLUDE = -I.
	
build:
	gcc -O3 -o encoder myBMPLib.c encoder.c -lm
	gcc -o3 -o decoder myBMPLib.c decoder.c -lm
	
demo0:
	./encoder 0 Kimberly.bmp R.txt G.txt B.txt dim.txt
	./decoder 0 Res0Kimberly.bmp R.txt G.txt B.txt dim.txt
	diff Res0Kimberly.bmp Kimberly.bmp
	
demo1:
	./encoder 1 Kimberly.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw
	
	./decoder 1 QRes1Kimberly.bmp Kimberly.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw
	diff QRes1Kimberly.bmp Kimberly.bmp || true
	
	./decoder 1 Res1Kimberly.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw
	diff Res1Kimberly.bmp Kimberly.bmp
	
demo2:
	./encoder 2 Kimberly.bmp ascii rle_code.txt
	./encoder 2 Kimberly.bmp binary rle_code.bin
	./decoder 2 QRes2aKimberly.bmp ascii rle_code.txt
	diff QRes2aKimberly.bmp QRes1Kimberly.bmp
	./decoder 2 QRes2bKimberly.bmp binary rle_code.bin
	diff QRes2bKimberly.bmp QRes1Kimberly.bmp


	
clean:
	-rm encoder decoder QRes1Kimberly.bmp Res0Kimberly.bmp Res1Kimberly.bmp R.txt G.txt B.txt Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw rle_code.txt rle_code.bin QRes2aKimberly.bmp QRes2bKimberly.bmp

.PHONY: build demo0 demo1 demo2 clean
