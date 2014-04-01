<h2>A light-weight and simple network programming framework</h2>

Time and I/O Events will be provided in this framework. 

And this framework is a simple trial to construct a push-notification server(as comet).

---------------------------------------------------------------------------------------
SealedServer is a asynchronous network, which supports I/O and Time events. I will provide support for signal event <span color="red">In near future</span>. 

<h2>Features</h2>
<ul>
	<li>Mutliple IO Workers Threads + One Demux Thread</li>
	<li>Reactor pattern for event-driven</li>
	<li>Support asynchronous I/O and Time events</li>
	<li>Support EPoll and Select.</li>
	<li>Provide a simple Garbage Collection. Mainly taken the methods nmstl.</li>
	<li>Only header files provided. Don't need pre-build. </li>
</ul>

Our work is mainly inspired by <a href="http://dirlt.com/‎">dirlt.com</a>, <a href="http://code.google.com/p/hpserver/">hpserver</a>, <a href="https://code.google.com/p/leveldb/"><span color="red">nmstl</span></a> and <a href="https://github.com/chenshuo/muduo">muduo</a>. Thanks again for open-source group.

<h2>Usage</h2>
How to use it? I have written a simple server-client in directory `tests`. Please find the detail by read codes.

<h2>Problems</h2>
There are many bugs exists in the project.
<ul>
	<li>close unexpectedly</li>
	<li>cLose expectedly</li>
	<li>time event when socket has been closed</li>
	<li>how to give benchmark</li>
	<li>misunderstand multiple-threading programming</li>
	<li>...</li>
</ul>

<h2>Design Principle</h2>
<ul>
	<li>Design by manually. Don't use library directly. So thread library, atomic, callback, garbage collection and many others has been provided.</li>
</ul>
