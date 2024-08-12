QHexView
==========

This is Qt widget for display binary data in traditional hex-editor style. This widget doesn`t have any editing capabilities. Only viewing and copying.


GUI
-----
![2015-03-29 13 49 54](https://cloud.githubusercontent.com/assets/10683398/6884566/d438a350-d61a-11e4-9c4b-16f67f5fdf07.png)


Building the example 
-----

* cd  QHexView
* mkdir build
* cd build
* qmake ../example/qhexview.pro
* make


Usage
-----
	...
	QByteArray data;
	...
	QHexView *phexView = new QHexView;
	...
	phexView -> setData(data);
	...
