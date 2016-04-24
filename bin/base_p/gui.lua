function TestLayerFunc(message)
	GUI:getLayer("TestLayer"):getWidget("TestWidget"):AcceptMessage(message)
	GUI:getLayer("TestLayer"):getWidget("PanelWidget"):AcceptMessage(message)
end

function StartLayerFunc(message)
	GUI:getLayer("StartLayer"):getWidget("StartWidget"):AcceptMessage(message)
end