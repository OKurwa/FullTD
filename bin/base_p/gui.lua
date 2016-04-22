function TestLayerFunc(message)
	GUI:getLayer("TestLayer"):getWidget("TestWidget"):AcceptMessage(message)
end

function StartLayerFunc(message)
	GUI:getLayer("StartLayer"):getWidget("StartWidget"):AcceptMessage(message)
end