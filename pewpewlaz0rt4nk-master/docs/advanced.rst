Advanced Beams
**************

.. sectionauthor:: Lennart Grahl <lennart.grahl@gmail.com>

The :class:`.Beam` class provides you with even more ways to destroy your
friend's HTTP server. Provoke a huge amount of send calls to or shutdown the
socket at the end, it's up to you!

The examples in the following sections lack the keyword arguments `request` and
`response`. In the real world, you probably want to specify them as well.
Otherwise the beam would be... well, pretty useless.

.. _replacement-fields:

Replacement Fields
==================

.. versionadded:: 1.1.0

Curly bracket enclosed parts in the HTTP **request** like ``{host}`` will be
interpreted as replacement fields.

They represent the same values that have been passed to either the
:class:`.Laz0rCannon` or explicitly to a :class:`.Beam`.

Available fields:
    - `host`
    - `port`
    - `timeout`
    - `buffer`

The following example will replace both `host` and `port` in the request with
the equivalent keyword arguments that have been passed to the
:class:`.Laz0rCannon`.

.. code-block:: python

    # Initialise cannon and append a beam
    cannon = Laz0rCannon(host='0wnz0riz0r.example.com', port=12345)
    cannon += Beam(request='GET / HTTP/1.1\r\nHost: {host}:{port}\r\n\r\n')

If you want to use a curly bracket, use ``{{`` as an escape sequence for ``{``
and ``}}`` instead of ``}``.

You can read more about replacement fields and Python's format functionality in
this section: :ref:`formatstrings`.

Description
===========

.. versionadded:: 1.1.2

You can add a description to a specific :class:`.Beam` by using the keyword
`description`. It will be displayed next to the :class:`.Beam` number.

.. code-block:: python

    beam = Beam(description='An example description')

Host and Port
=============

A :class:`.Beam` can have it's own `host` and `port` keyword arguments. We don't
have any use case for this (at the moment), but you might have one.

.. code-block:: python

    beam = Beam(host='0wnz0riz0r.example.com', port=12345)

.. _shutdown:

Shutdown
========

To test how the server reacts after a socket shutdown, you can specify the
keyword argument `shutdown`. It can either be ``False`` (default) or contain one
of the constants ``SHUT_RD``, ``SHUT_WR`` or ``SHUT_RDWR`` from the
:mod:`socket` module.

=========== ================================================
`SHUT_RD`   Further receives from the socket are disallowed.
`SHUT_WR`   Further sends to the socket are disallowed.
`SHUT_RDWR` Further sends and receives are disallowed.
=========== ================================================

.. note:: Depending on the platform, shutting down one half of the connection
          can also close the opposite half.

.. code-block:: python

    beam = Beam(shutdown=socket.SHUT_WR)

.. _buffer_:

Buffer
======

With the `buffer_` (the underscore avoids a collision with the built-in function
:func:`buffer`) keyword argument you can define the buffer size in bytes. At
default, the buffer size is `4096` bytes.

The buffer size controls the amount of calls to :meth:`~socket.socket.send` and
:meth:`~socket.socket.recv` on the socket. For example, a buffer size of `1`
would result in a single byte being sent on each call to
:meth:`~socket.socket.send`.

.. note:: Keep in mind that sockets are controlled by the operating system which
          can still decide to buffer the data internally.

.. code-block:: python

    beam = Beam(buffer_=1)

Timeout
=======

.. versionadded:: 1.1.0

The keyword argument `timeout` specifies the amount of seconds until a read from
or a write to the socket will time out.

This keyword can also be used on the :class:`.Laz0rCannon` to specify a default
value for each :class:`.Beam`.

.. code-block:: python

    beam = Beam(timeout=10)
