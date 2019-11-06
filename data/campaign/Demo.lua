-- Demo

reset()

conf.sv_timelimit = 0
conf.sv_fraglimit = 0
conf.sv_nightmode = true
loadmap("campaign/Demo/maps/part1.map")


local time = 2
pushcmd(function() message("������!") end, time)

time = time + 3;
pushcmd(function() message("��� ��������� ������������ ������������ ����������� �����") end, time)

time = time + 3;
pushcmd(function() message("������ ����� ��������� ������ �����") end, time)

time = time + 2;
pushcmd(function() conf.sv_nightmode = false; loadmap("campaign/Demo/maps/part1.map") end, time)

time = time + 1;
pushcmd(function() message("��� ��� :)") end, time)

time = time + 3;
pushcmd(function() message("������������� �������� ��������� ���:") end, time)

time = time + 3;
pushcmd(function() message("- ��������� �������") end, time)

-- �������� ���� ����� ����...
time = time + 2;
pushcmd(function()
  for i = 1, 10 do
    pushcmd(function() actor("wall_brick", 272 + i*32, 144 + i*32) end, 1 + 0.1 * i)
  end
  for i = 1, 10 do
    pushcmd(function() actor("wall_brick", 400 + i*32, 144 + i*32) end, 0.1 * i)
  end
end, time)

-- ... � �����������
time = time + 1;
pushcmd(function() actor("turret_cannon", 288, 288, {name="obj1", team=1, on_destroy="damage(1000, 'obj2'); user.TaskComplete1()"}) end, time)

time = time + 0.5;
pushcmd(function() actor("turret_rocket", 608, 224, {name="obj2", team=1, on_destroy="damage(1000, 'obj1')"}) end, time)

time = time + 1;
pushcmd(function() message("- ������������� ��������� ����� ����� ���������") end, time)

time = time + 2;
pushcmd(function() message("   (���������� ����� ���� �� �����������)") end, time)

time = time + 1;
pushcmd(function() actor("respawn_point", 100, 300, {team=1}); user.p=service("player_local", {nick="������", team=1}) end, time)

time = time + 3;
pushcmd(function()
--          freeze(true)              -- ������ ���� �� ����� �� ����� ������ �������
          service("msgbox", {
              on_select=[[
                  user.w=actor(({"weap_cannon", "weap_minigun", "weap_plazma"})[n], 150, 200)
--                  freeze(false)    -- ������� � �����
              ]],
              text="����� ������ �� �������������?", 
              option1="���.�����",
              option2="�������",
              option3="������"
          })
        end, time)


function user.TaskComplete1()
  
  time = 2 -- ���� �� ������� ������ ����� ���������� �������, ������� ���� �������� ������
  pushcmd(function() message("�������!") end, time)

  time = time + 3;
  pushcmd(function() message("������ ������ � ����...") end, time)
  
  time = time + 2;
  pushcmd(function() kill(user.p) end, time)
  
  time = time + 3;
  pushcmd(function() message(" ...� ������ ���� ���� ������") end, time)
  
  time = time + 2;
  pushcmd(function() kill(user.w) end, time)

  time = time + 3;
  pushcmd(function() message("���������� ����� ������� ������") end, time)
  
  time = time + 3;
  pushcmd(function() conf.sv_nightmode=true; loadmap("campaign/Demo/maps/part2.map") end, time)
  
  time = time + 4;
  pushcmd(function() message("��� ��������� ��������������") end, time)

  time = time + 4;
  pushcmd(function()
            message("� ������ ��������� ���������. ��������� ���!")
            service("player_local", {nick="����� 007"})
          end, time)
end


function user.OnDestroyGenerator()
  time = 0
  for i=1,6 do
    time = time + 1
    pushcmd(function()
              for j=1,4 do pset("p"..i.."-"..j, "active", 0) end
            end, time)
  end
  
  time = time + 1
  pushcmd(function() message("�������! ���� ������� ��� �����.") end, time)
  
  time = time + 3
  pushcmd(function() message("��� ��� ������ ������ �� ���������� ��������.") end, time)
  
  time = time + 3
  pushcmd(function()
--            freeze(true)
            service("msgbox", {
                   on_select=[[
                     dofile("scripts/init.lua")
                   ]],
                   text="  �������������� ���������� �� ��������:\n    http://ru.zod.wikia.com\n\n" ..
                   "  ��������� ������� �� ������:\n    http://zod.borda.ru\n\n" .. 
                   "� ���� ��� Insert.\n  ������� �� ���������! =)"
            })
          end, time)
  
  
end

