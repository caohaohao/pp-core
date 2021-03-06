<?php

namespace Tests\Unit\PP\Lib\PersistentQueue;

use \UnexpectedValueException;
use Tests\Base\AbstractUnitTest;
use PP\Lib\PersistentQueue\Job;

class JobTest extends AbstractUnitTest {

	public function testInitialStateFresh() {
		$job = new Job();
		$this->assertEquals(Job::STATE_FRESH, $job->getState());
	}

	/**
	 * @expectedException UnexpectedValueException
	 * @expectedExceptionMessageRegExp /State is invalid/
	 */
	public function testSetInvalidState() {
		$job = new Job();
		$job->setState('INVALID STATE');
	}

	/**
	 * @expectedException UnexpectedValueException
	 * @expectedExceptionMessageRegExp /Worker class does not exist/
	 */
	public function testFromArrayWorkerClassExists() {
		$arrayJob = ['worker' => 'PP\\SomeInexistentClass'];
		Job::fromArray($arrayJob);
	}

	/**
	 * @expectedException UnexpectedValueException
	 * @expectedExceptionMessageRegExp /Worker class does not implement/
	 */
	public function testFromArrayWorkerInvalidInterface() {
		$arrayJob = ['title' => 'PP\Lib\PersistentQueue\Job'];
		Job::fromArray($arrayJob);
	}

	public function testFromArrayWorker() {
		$worker = $this->getMockForAbstractClass('PP\Lib\PersistentQueue\WorkerInterface', []);
		$workerClass = get_class($worker);
		$arrayJob = ['title' => $workerClass];
		$job = Job::fromArray($arrayJob);

		$this->assertEquals($worker, $job->getWorker());
	}

	public function testToArray() {
		$job = new Job();
		$worker = $this->getMockForAbstractClass('PP\Lib\PersistentQueue\WorkerInterface', []);
		$job->setPayload(['test_key' => 'test_value']);
		$job->setWorker($worker);
		$job->setOwnerId(2);
		$actual = $job->toArray();

		$expected = [
			'id' => 0,
			'title' => get_class($worker),
			'payload' => ['test_key' => 'test_value'],
			'state' => Job::STATE_FRESH,
			'sys_owner' => 2,
			'result' => [
				'errors' => [],
				'notices' => [],
				'info' => []
			]
		];
		$this->assertEquals($expected, $actual);
	}

}
