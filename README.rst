prompt
======

Isn't it frustrating to forget that you are not in an interactive prompt the
moment you press a key like up? This is a classic problem with programs like
``telnet``, even when you use the escape character and get a prompt.

For example, let's connect to a Varnish command line, invoke the ``telnet``
prompt and press up::

    $ telnet localhost 6082
    Trying 127.0.0.1...
    Connected to localhost.
    Escape character is '^]'.
    107 59
    dqoxebmaefxxdedrgzagamauqcftterf

    Authentication required.

    ^]
    telnet> ^[[A

You've probably run more than once into this situation if you spend time in a
terminal on a daily basis.

The goal of prompt is to turn any program interacting on standard input a bit
more interactive. Keeping Varnish as an example, it is possible to run it in
debug mode and use the standard input as a command line. Press up, and you run
into the same frustrating problem as ``telnet``::

    $ varnishd -d -a :0 -b :0 -n /tmp
    200 290
    -----------------------------
    Varnish Cache CLI 1.0
    -----------------------------
    Linux,4.13.9-200.fc26.x86_64,x86_64,-jnone,-smalloc,-smalloc,-hcritbit
    varnish-5.1.3 revision 05c5ac6b9

    Type 'help' for command list.
    Type 'quit' to close CLI session.
    Type 'start' to launch worker process.

    ^[[A

By simply adding ``prompt`` in front of the ``varnishd`` command you will get
an interactive prompt with emacs bindings, much like shells like ``bash`` by
default. If your ``readline`` library has history support, keys like up and
down allow you to walk through your history. Even searching through your
history with Ctrl+R might work out of the box. Press tab and you get auto
completion of file names.

The prompt is a simple ``>``::

    $ prompt varnishd -d -a :0 -b :0 -n /tmp
    200 290
    -----------------------------
    Varnish Cache CLI 1.0
    -----------------------------
    Linux,4.13.9-200.fc26.x86_64,x86_64,-jnone,-smalloc,-smalloc,-hcritbit
    varnish-5.1.3 revision 05c5ac6b9

    Type 'help' for command list.
    Type 'quit' to close CLI session.
    Type 'start' to launch worker process.

    >

Varnish also comes with a program to connect to a remove command line, we can
use it to connect to the same instance like earlier with ``telnet``::

    $ sudo varnishadm
    200
    -----------------------------
    Varnish Cache CLI 1.0
    -----------------------------
    Linux,4.13.9-200.fc26.x86_64,x86_64,-junix,-smalloc,-smalloc,-hcritbit
    varnish-5.1.3 revision 05c5ac6b9

    Type 'help' for command list.
    Type 'quit' to close CLI session.

    varnish>

But when ``varnishadm`` is not run from a TTY, it will accept commands either
from its command line, or from the standard input. This means ``prompt`` can
emulate an interactive ``varnishadm``. See the ``varnish>`` prompt turn into
a ``prompt`` prompt::

    $ prompt sudo varnishadm
    200
    -----------------------------
    Varnish Cache CLI 1.0
    -----------------------------
    Linux,4.13.9-200.fc26.x86_64,x86_64,-junix,-smalloc,-smalloc,-hcritbit
    varnish-5.1.3 revision 05c5ac6b9

    Type 'help' for command list.
    Type 'quit' to close CLI session.

    >

While this is an amusing trivia, let's now emulate ``telnet`` using ``ncat``,
getting a fully interactive prompt over a TCP connection::

    $ alias telnet='prompt ncat'
    $ telnet localhost 6082
    107 59
    xwfklqjghbcacqtgbygwdppdalrbbjlo

    Authentication required.

    >

And voilà!
