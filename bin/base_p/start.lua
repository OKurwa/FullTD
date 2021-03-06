Include("gui.lua")
--
-- ��� ����������� �������� ��������: ����� ��������, �������� � �.�.
-- ����� ����� � ��������� �������� �� �����.
--
-- ����������� �������� �������� � ����� � ������ �����������
-- �������� UploadResourceGroup, ������� ���������� ����.
--
LoadResource("Resources.xml")
LoadFreeTypeText("text.xml")

--
-- �������� ��������, ��������� � ��������� ������.
--
LoadEffects("Example2_Flames.xml")
LoadEffects("Example3_FindObject.xml")
LoadEffects("TestEffects.xml")
--
-- �������� ����.
--
-- ���� ��������� ��� ����� (���������� ������������ ���� �������� � �.�.),
-- �� ������ Draw() � Update() �������� ���������� ������ �����, ����� ����
-- �������� �� �����. � ������ ���������� �� ����� � �������� ���� ���������� �����
-- AcceptMessage � ���������� "Init".
--
GUI:LoadLayers("Layers.xml")

--
-- ����������� �������� ������ ��������: ��������� ������� �������, �����������
-- ����������� � ����� � �.�. ��� ���������� ��������.
--
UploadResourceGroup("TestGroup")

--
-- ���� ������� �� �����.
--
-- � ���� ������ � �������� ���� ���� ��� ���������� ����� AcceptMessage("Init")
-- � �������� ���������� ������ Draw() � Update() � ������ �����.
--
Screen:pushLayer(GUI:getLayer("TestLayer"))
Screen:pushLayer(GUI:getLayer("StartLayer"))
