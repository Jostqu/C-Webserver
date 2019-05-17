Cann0n Usage - A Quick Tutorial
*******************************

.. sectionauthor:: Lennart Grahl <lennart.grahl@gmail.com>

The following examples will explain everything you need to know to test an HTTP
server.

Don't worry if you don't know completely what's going on here, you don't have to
learn Python to write tests (but you should because Python is absolutely
amazing).

Preparations
============

Create a new text file, name it `pwn.py` and open it in your favourite code
editor.

To execute the python script you just have to open a terminal and change to the
directory where `pwn.py` is located. The following command will then execute
your script.

.. code-block:: none

    python pwn.py

Command Line Arguments
======================

First of all, we have to import a few things from the module
:mod:`pewpewlaz0rt4nk`. In this tutorial we will only use the classes
:class:`.Laz0rCannon` and :class:`.Beam` of this module.

We also need the module :mod:`sys` to evaluate arguments passed as command line
arguments. In case you're using Python 2, you also need to include the
``unicode_literals`` module to treat all strings as unicode by default.

.. code-block:: python

    from __future__ import unicode_literals  # Only for Python 2

    import sys

    from pewpewlaz0rt4nk import Laz0rCannon, Beam

Now, we set the default variables `host` and `port`.
These will be used if no or more than two arguments have been passed.

.. code-block:: python

    host = 'localhost'
    port = 31337

There is no point in having default variables if we don't overwrite them on a
specific condition, right?

The following code will check whether exactly three command line arguments
exist. If they exist, host and port will be
`unpacked <http://anothercomputingblog.blogspot.de/2010/05/functional-unpacking-style.html>`_
from a list of arguments, thus overwriting `host` and `port` in this process.

.. code-block:: python

    # Overwrite host and port if command line arguments have been passed
    if len(sys.argv) == 3:
        # Unpack the list sys.argv, ignore the first item but extract
        # the following two
        _, host, port = sys.argv
        port = int(port)

Initialising the Cann0n
=======================

Evaluating the command line arguments in the previous section
`Command Line Arguments`_ was just for convenience.

As the sections build upon one another we need the code of the previous section
as basis before we can proceed:

.. code-block:: python

    from __future__ import unicode_literals  # Only for Python 2

    import sys

    from pewpewlaz0rt4nk import Laz0rCannon, Beam

    host = 'localhost'
    port = 31337

    # Overwrite host and port if command line arguments have been passed
    if len(sys.argv) == 3:
        # Unpack the list sys.argv, ignore the first item but extract
        # the following two
        _, host, port = sys.argv
        port = int(port)

Now, we create an instance of class :class:`.Laz0rCannon` which provides the
main functionality to test an HTTP server.

.. code-block:: python

    # Initialise cannon
    cannon = Laz0rCannon(host=host, port=port)

The ``host=host`` and ``port=port`` part might look odd to you. These are called
:ref:`tut-keywordargs`.

One of the cool things about keyword arguments is that they do not have to be
passed in a specific order. Therefore, ``Laz0rCannon(port=port, host=host)``
would do exactly the same.

A Cann0n Beam
=============

Now, we will create a beam (test case) for the laz0r cannon. A test consists of
both an HTTP request and the expected HTTP response.

We call this class a `Beam` because Pewpewlaz0rt4nks shoot laz0r beams to
pulverize vulnerable servers (and your friend's self-esteem).

Request
-------

An HTTP request is a simple string containing a valid or invalid HTTP request.
For example, to test a specific feature like authorisation or to check whether
an expected status code was returned.

Due to the fact that HTTP requests use new lines to separate headers, you have
to specify them in your request. You can do this by using the escape sequence
``\r\n`` (or just ``\n`` if you want to provoke a bad request).

A valid request could look like this:

.. code-block:: python

    request = 'GET /path/to/file HTTP/1.1\r\nHost: localhost\r\n\r\n'

Response
--------

.. note:: Read this section thoroughly because it contains vital information
          about the HTTP response validation mechanism. Keep these mechanisms in
          mind while writing beams.

An expected HTTP response is a list of strings containing valid responses. The
**beginning of each line** of the actual response will be compared against the
current line of the expected response.

In addition to that, the **order** of the actual response lines has to be
**exactly the same** as the order of the expected response lines but the amount
of actual response lines may be longer.

The following three sections will explain these mechanism with examples.

Response line comparison
^^^^^^^^^^^^^^^^^^^^^^^^

Let's define an expected response line as ``HTTP/1.0 400 Bad Request``.

A response line is valid if the actual response line...

* is exactly ``HTTP/1.0 400 Bad Request``.
* begins with the same as above but is longer, like
  ``HTTP/1.0 400 Bad Request Yes Indeed Very Bad Request``.

A response line is invalid if the actual response line...

* is shorter than the expected response, like ``HTTP/1.0 400 Bad``.
* does not match against the expected response, like ``HTTP/1.0 404 Not Found``.

Response order comparison
^^^^^^^^^^^^^^^^^^^^^^^^^

Let's define the expected response as the following:

.. code-block:: python

    response = ['HTTP/1.0 200', 'Date: ', 'Server: Droelfundzwuenfzig Server']

A valid actual response could look like this:

.. code-block:: none

    HTTP/1.0 200 OK
    Date: Tue, 01 April 2014 22:38:34 GMT
    Server: Droelfundzwuenfzig Server (version droelf*zwuenf+pi)
    Content-Type: text/html
    ...

Note that the actual response contains more headers than we have defined. This
will still be interpreted as a valid response.

But if the second and third header of the actual response would be reversed, the
response would be invalid:

.. code-block:: none

    HTTP/1.0 200 OK
    Server: Droelfundzwuenfzig Server droelf*zwuenf+pi
    Date: Tue, 01 April 2014 22:38:34 GMT
    Content-Type: text/html
    ...

Response length comparison
^^^^^^^^^^^^^^^^^^^^^^^^^^

As seen in the section above, an actual response is still valid if it is longer
than the expected response. But if the actual response is shorter, the response
is invalid.

Let's define the expected response as the following:

.. code-block:: python

    response = ['HTTP/1.0 200', 'Date: ', 'Server: ']

And let the actual response look like this:

.. code-block:: none

    HTTP/1.0 200 OK
    Date: Tue, 01 April 2014 22:38:34 GMT

As you can see, although the first response lines are both valid, the third line
is missing in the actual response. The result is an invalid response.

Creating a Beam
---------------

Now that we have understood how a request looks like and how the mechanism
behind the response comparison works, we can write our first test case.

The following code creates an instance of class :class:`.Beam` that awaits two
keyword arguments `request` and `response`. We have already learned how to
define these, so this should be an easy task.

.. code-block:: python

    # Append beams to the cannon
    cannon += Beam(
        request='GET /index.html HTTP/1.1\r\nHost: {host}\r\nConnection: close\r\n\r\n',
        response=['HTTP/1.1 200']
    )

This example represents a valid request as long as the file `index.html`
actually exists in the HTTP server's document root.

Have you noticed the curly bracket enclosed part ``{host}`` in the request
string? This is called a `replacement field` and will be automatically replaced
by the keyword argument we have passed to the :class:`.Laz0rCannon`.

You can learn more about these fields in the section :ref:`replacement-fields`.

Here is another example.

.. code-block:: python

    cannon += Beam(
        request='SHRED /index.html HTTP/1.1\r\nHost: {host}\r\n\r\n',
        response=['HTTP/1.1 501']
    )

Notice the difference? Yep, `SHRED`... I doubt there is an implementation of
that request method. Therefore, we expect status `501 Not Implemented` in the
response.
    
Of course, even more beams can be added in this manner.

Pewpew!
=======

Alright, the :class:`.Laz0rCannon` has been idle long enough. **It's time to
fire our laz0r beams!** The `pewpew` process can be started like this:

.. code-block:: python

    # PEWPEW!
    cannon.pewpew()

After executing the complete code of this tutorial you should see a
self-explanatory summary of the test cases.

It is possible to call `pewpew` multiple times. To reset the statistics and let
the laz0r cool down a bit you can call `reset` on the cannon.
The following example will show you how to use both methods in a loop.

.. code-block:: python

    # Massive PEWPEW!
    for _ in range(100):
        cannon.pewpew()
        cannon.reset()

Now, you can start pwning your friend's server. May the `pewpew` be with you!

Tutorial Code
=============

The complete code of this tutorial.

.. code-block:: python

    from __future__ import unicode_literals  # Only for Python 2

    import sys

    from pewpewlaz0rt4nk import Laz0rCannon, Beam

    host = 'localhost'
    port = 31337

    # Overwrite host and port if command line arguments have been passed
    if len(sys.argv) == 3:
        # Unpack the list sys.argv, ignore the first item but extract
        # the following two
        _, host, port = sys.argv
        port = int(port)

    # Initialise cannon
    cannon = Laz0rCannon(host=host, port=port)

    # Append beams to the cannon
    cannon += Beam(
        request='GET /index.html HTTP/1.1\r\nHost: {host}\r\nConnection: close\r\n\r\n',
        response=['HTTP/1.1 200']
    )
    cannon += Beam(
        request='SHRED /index.html HTTP/1.1\r\nHost: {host}\r\n\r\n',
        response=['HTTP/1.1 501']
    )

    # PEWPEW!
    cannon.pewpew()
