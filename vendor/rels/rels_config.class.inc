<?php
	class RELS_Config {

		var $comparsionOperands = array(
			'='   => '__op_eq',
			'=~'  => '__op_similar',
			'!='  => '__op_ne',
			'!=~' => '__op_not_similar',
			'<'   => '__op_lt',
			'<='  => '__op_le',
			'>'   => '__op_gt',
			'>='  => '__op_ge'
		);
		
		var $logicOperands = array(
			'|' => '__op_or',
			'&' => '__op_and'
		);

		var $exceptions = array(
			'UNKNOWNEXCEPTION'       => '����������� ������',
			
			'REQMUSTBESTRING'        => '���������� ������ �� �������� �������',
			'EMPTYREQ'               => '������ ������',
			'NOFIELDSGIVEN'          => '��� ������ ������� RELS �� ������� ������ ����������� ����������!',
			
			'NOKNOWNFILEDSGIVEN'     => '� ������� ����������� ����������� ����������',
			
			'COMMONSYNTAXERROR'      => '������ ����������: ������������ ����������, �������� ��� ���������; ������� - %d, ����� ������� "%s"',
			'BRACKETSYNTAXERROR'     => '������ ����������: � ������� %d ���������� ������������ ������',
			'LOGICSYNTAXERROR'       => '������ ����������: � ������� %d ��������� ������������ ���������� ������',
			'ATENDSYNTAXERROR'       => '������ ����������: � ������� ������������ ���������� ������ ��� ������ ���������� ��������',
			
			'UNSUPPORTEDOPERAND'     => '������ %s: ������ ������� ������� RELS �� ������������ �������� %s',
			'REGEXPCANNOTBENUMERIC'  => '������ %s: �������� "%s" ������������ ������ ���������� ���������, ��� ������� �������� ��������� ���������� ������������ �������� ����������� ���������'
		);
		
		var $testingFSMTable = array(
			"begin" =>
				array(
					"openbracket"  => true,
					"closebracket" => false,
					"expression"   => true,
					"logic"        => false,
					"end"          => false
				),
			"openbracket" =>
				array(
					"openbracket"  => true,
					"closebracket" => false,
					"expression"   => true,
					"logic"        => false,
					"end"          => false
				),
			"closebracket" =>
				array(
					"openbracket"  => false,
					"closebracket" => true,
					"expression"   => false,
					"logic"        => true,
					"end"          => true
				),
			"expression" =>
				array(
					"openbracket"  => false,
					"closebracket" => true,
					"expression"   => false,
					"logic"        => true,
					"end"          => true
				),
			"logic" =>
				array(
					"openbracket"  => true,
					"closebracket" => false,
					"expression"   => true,
					"logic"        => false,
					"end"          => false
				)
		);
	
	}
?>