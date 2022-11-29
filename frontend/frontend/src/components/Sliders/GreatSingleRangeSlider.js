import React, { useState, useEffect } from 'react';
import './GreatSingleRangeSlider.css';

const GreatSingleRangeSlider = (props) => {
	let baseClassName = 'great-range-slider';
	const min = parseFloat(props.min || 0);
	const max = parseFloat(props.max || 100);
	const step = parseFloat(props.step || 5);
	const stepCount = (max - min) / step;
	let ruler = props.ruler ?? true;
	let label = props.label ?? true;

	ruler = ruler === 'false' || !ruler ? false : true;
	label = label === 'false' || !label ? false : true;

	const preventWheel = props.preventWheel === 'true' || props.preventWheel === true || false;
	const [minValue, set_minValue] = useState(parseFloat(props.minValue || 25));
	const maxValue = parseFloat(props.maxValue || 75)
	const [barMin, set_barMin] = useState(((minValue - min) / (max - min)) * 100);
	const [FirstTimeUseEffect, setFirstTimeUseEffect] = useState(true);
	const getRandomID = (len) => {
		const chars = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
		len = len || 10;
		let randomID = '_';
		for (let i = 0; i < len; i++) {
			randomID += chars[Math.floor(Math.random() * chars.length)];
		}
		return randomID;
	};
	const ID = 'MultiRangeSlider' + getRandomID(15);

	let barBox = null;
	let startX = null;
	let barValue = 0;
	let bar = null;
	const onInputMinChange = (e) => {
		let _minValue = parseFloat(e.target.value);
		if (_minValue > maxValue - step) {
			_minValue = maxValue - step;
		}
		set_minValue(_minValue);
		let _barMin = ((_minValue - min) / (max - min)) * 100;
		set_barMin(_barMin);
		triggerInput(_minValue, maxValue);
	};
	const onLeftThumbMousedown = (e) => {
		startX = e.clientX;
		if (e.type === 'touchstart') {
			if (e.touches.length === 1) {
				startX = e.touches[0].clientX;
			} else {
				return;
			}
		}

		barValue = minValue;
		bar = e.target.parentNode;
		barBox = bar.getBoundingClientRect();
		document.addEventListener('mousemove', onLeftThumbMousemove);
		document.addEventListener('mouseup', onLeftThumbMouseup);
		document.addEventListener('touchmove', onLeftThumbMousemove);
		document.addEventListener('touchend', onLeftThumbMouseup);
		bar.classList.add('active');
	};
	const onLeftThumbMousemove = (e) => {
		let clientX = e.clientX;
		if (e.type === 'touchmove') {
			clientX = e.touches[0].clientX;
		}
		let dx = clientX - startX;
		let per = dx / barBox.width;
		let val = barValue + (max - min) * per;
		let strSetp = '' + step;
		let fixed = 0;
		strSetp.indexOf('.') >= 0 && (fixed = 2);
		val = parseFloat(val.toFixed(fixed));
		if (val < min) {
			val = min;
		} else if (val > maxValue - step) {
			val = maxValue - step;
		}
		set_minValue(val);
		let _barMin = ((val - min) / (max - min)) * 100;
		set_barMin(_barMin);
		triggerInput(val, maxValue);
	};
	const onLeftThumbMouseup = (e) => {
		document.removeEventListener('mousemove', onLeftThumbMousemove);
		document.removeEventListener('mouseup', onLeftThumbMouseup);
		document.removeEventListener('touchmove', onLeftThumbMousemove);
		document.removeEventListener('touchend', onLeftThumbMouseup);
		bar.classList.remove('active');
	};

	const triggerInput = (minValue, maxValue) => {
		let retObj = { min, max, minValue, maxValue };
		props.onInput && props.onInput(retObj);
		props.onChange && props.onChange(retObj);
	};
	useEffect(() => {
		FirstTimeUseEffect &&
			(() => {
				document.querySelector('#' + ID).addEventListener('wheel', (e) => {
					if (preventWheel === true || (!e.shiftKey && !e.ctrlKey)) {
						return;
					}
					e.preventDefault();
				});
				setFirstTimeUseEffect(false);
			})();
	}, [FirstTimeUseEffect, preventWheel, ID]);

	return (
		<div id={ID} className={baseClassName}>
			<div className='bar'>
				<div className='bar-left' style={{ width: barMin + '%' }}></div>
				<input className='input-type-range input-type-range-min' type='range' min={min} max={max} step={step} value={minValue} onInput={onInputMinChange} />
				<div className='thumb thumb-left' onMouseDown={onLeftThumbMousedown} onTouchStart={onLeftThumbMousedown}>
					<div className='min-value'>{minValue}</div>
				</div>
				<div className='bar-inner'>
				</div>
			</div>
			{ruler && (
				<div className='ruler'>
					{[...Array(stepCount)].map((e, i) => (
						<div key={i} className='ruler-rule'></div>
					))}
				</div>
			)}
			{label && (
				<div className='label'>
					<div className='label-min'>{min}</div>
					<div className='label-max'>{max}</div>
				</div>
			)}
		</div>
	);
};

export default GreatSingleRangeSlider;
