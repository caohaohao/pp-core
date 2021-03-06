<?php

namespace Tests\Unit\PP\Lib\PersistentQueue;

use Tests\Base\AbstractUnitTest;
use PP\Lib\PersistentQueue\Job;
use PP\Lib\PersistentQueue\Queue;

class QueueTest extends AbstractUnitTest {

	/**
	 * @var Queue
	 */
	protected $queue;

	/**
	 * @var \PHPUnit_Framework_MockObject_MockObject
	 */
	protected $db;

	/**
	 * @var \PHPUnit_Framework_MockObject_MockObject
	 */
	protected $app;

	/**
	 * @before
	 */
	public function before() {
		$this->app = $this->getMockBuilder('PXApplication')
			->disableOriginalConstructor()
			->setMethods(['initContentObject'])
			->getMock();

		$this->app->types = [
			Queue::JOB_DB_TYPE => Queue::JOB_DB_TYPE
		];

		$this->db = $this->getMockBuilder('PXDatabase')
			->disableOriginalConstructor()
			->setMethods(['addContentObject', 'modifyContentObject', 'getObjectsByFieldLimited'])
			->getMock();

		$this->queue = new Queue($this->app, $this->db);
	}

	public function testStartJob() {
		$job = new Job();
		$expected = (new Job())->setState(Job::STATE_IN_PROGRESS);

		$this->db->expects($this->once())
			->method('modifyContentObject')
			->with(
				$this->equalTo(Queue::JOB_DB_TYPE),
				$this->equalTo($expected->toArray())
			);

		$this->queue->startJob($job);
	}

	public function testFailJob() {
		$job = new Job();
		$expected = (new Job())->setState(Job::STATE_FAILED);

		$this->db->expects($this->once())
			->method('modifyContentObject')
			->with(
				$this->equalTo(Queue::JOB_DB_TYPE),
				$this->equalTo($expected->toArray())
			);

		$this->queue->failJob($job);
	}

	public function testFinishJob() {
		$job = new Job();
		$expected = (new Job())->setState(Job::STATE_FINISHED);

		$this->db->expects($this->once())
			->method('modifyContentObject')
			->with(
				$this->equalTo(Queue::JOB_DB_TYPE),
				$this->equalTo($expected->toArray())
			);

		$this->queue->finishJob($job);
	}

	public function testGetFreshJobs() {
		$limit = 2;
		$jobOne = (new Job())
			->setPayload(['test_id' => 1])
			->setId(1)
			->setWorker($this->getMockForAbstractClass('PP\Lib\PersistentQueue\WorkerInterface', []));

		$jobTwo = (new Job())
			->setPayload(['test_id' => 2])
			->setId(2)
			->setWorker($this->getMockForAbstractClass('PP\Lib\PersistentQueue\WorkerInterface', []));

		$this->db->expects($this->once())
			->method('getObjectsByFieldLimited')
			->with(
				$this->equalTo(Queue::JOB_DB_TYPE),
				$this->equalTo(true),
				$this->equalTo('state'),
				$this->equalTo(Job::STATE_FRESH),
				$this->equalTo($limit),
				$this->equalTo(0)
			)
			->willReturn([$jobOne->toArray(), $jobTwo->toArray()]);

		$jobs = $this->queue->getFreshJobs($limit);

		$this->assertEquals([$jobOne, $jobTwo], $jobs);
	}

	public function testAddJob() {
		$job = (new Job())
			->setWorker($this->getMockForAbstractClass('PP\Lib\PersistentQueue\WorkerInterface', []));

		$stub = ['stub_param' => 'stub_value'];

		$this->app->expects($this->once())
			->method('initContentObject')
			->with($this->equalTo(Queue::JOB_DB_TYPE))
			->willReturn($stub);

		$this->db->expects($this->once())
			->method('addContentObject')
			->with(
				$this->equalTo(Queue::JOB_DB_TYPE),
				array_merge($stub, $job->toArray())
			)
			->willReturn(1);

		$id = $this->queue->addJob($job);

		$this->assertGreaterThan(0, $id);
	}

}
