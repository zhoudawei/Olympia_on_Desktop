onmessage = function(event) {
    if (event.data == "noport") {
        if (!event.ports)
            testPassed("event.ports is null when no port sent");
        else
            testFailed("event.ports should be null when no port sent");
    } else if (event.data == "zero ports") {
        if (!event.ports)
            testPassed("event.ports is null when empty array sent");
        else
            testFailed("event.ports should be null when empty array sent");
    } else if (event.data == "two ports") {
        if (!event.ports) {
            testFailed("event.ports should be non-null when ports sent");
            return;
        }
        if (event.ports.length == 2)
            testPassed("event.ports contains two ports when two ports sent");
        else
            testFailed("event.ports contained " + event.ports.length + " when two ports sent");
    } else if (event.data == "failed ports") {
        if (event.ports.length == 2)
            testPassed("event.ports contains two ports when two ports re-sent after error");
        else
            testFailed("event.ports contained " + event.ports.length + " when two ports re-sent after error");
    } else if (event.data == "done") {
        postMessage("done");
        event.ports[0].postMessage("done");
    } else
        testFailed("Received unexpected message: " + event.data);
};

function testPassed(msg)
{
    postMessage("PASS"+msg);
}

function testFailed(msg)
{
    postMessage("FAIL"+msg);
}
