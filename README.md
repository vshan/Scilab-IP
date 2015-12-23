# Scilab Image Processing Module
Image Processing codes for Scilab

## Instructions
1. Get the `Scilab-Image-Processing-Toolbox-master` folder, put it in some path $P.
2. Copy relevant files to the folder.

```bash
$ git clone https://github.com/vshan/Scilab-IP.git
$ cp Scilab-IP/src/* $P/sci_gateway/cpp/
$ cp Scilab-IP/help/* $P/help/en_US/
```

3. Run Scilab, inside the Scilab console type:

```bash
--> cd $P/
--> exec builder.sce;
--> exec loader.sce;
```

4. Example of a function:

```bash
--> im = imread('/path/to/image.jpg');
--> out = imhmax(im, 80);
--> imshow(out);
```