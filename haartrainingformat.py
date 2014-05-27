#!/usr/bin/python

import os
import re
import argparse

def main():
	parser = argparse.ArgumentParser()

	parser.add_argument("--clipdir", metavar = "CLIPDIR", 
					help = "Path to a directory containing the clipped images "
							"in the format default imageclipper output format: "
							"<imgout_format = %%d/imageclipper/\%%i.%%e_%%04r_%%04x_%%04y_%%04w_%%04h.png>")

	parser.add_argument("--path", metavar = "PATH", default = "",
					help = "The path to prepend to the image name in the output.")

	args = parser.parse_args()

	clip_images = os.listdir(args.clipdir)

	for img in clip_images:
		if os.path.isdir(os.path.join(args.clipdir, img)):
			continue

		try:
			m = re.match("(.*?\..*?)_(-?\d+)_(-?\d+)_(-?\d+)_(-?\d+)_(-?\d+)\..*?", img)
			basename = m.group(1)
			r = max(0, int(m.group(2)))
			x = max(0, int(m.group(3)))
			y = max(0, int(m.group(4)))
			w = max(0, int(m.group(5)))
			h = max(0, int(m.group(6)))

			base_img_path = os.path.join(args.path, basename)

			if (os.path.exists(base_img_path)):
				print("%s %d %d %d %d %d" % (base_img_path, 1, x, y, w, h))
		except Exception as ex:
			print ex
			return


if __name__ == '__main__': main()
