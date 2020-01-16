//
//  DialView.swift
//  OpenDuo
//
//  Created by CavanSu on 2020/1/13.
//  Copyright © 2020 Agora. All rights reserved.
//

import UIKit

protocol DialViewDelegate: NSObjectProtocol {
    func dialView(_ dialView: DialView, didPressedButton number: String)
}

class DialView: UIView {
    class NumberButton: UIButton {
        var numberLabel: UILabel
        var charLabel: UILabel
        
        override init(frame: CGRect) {
            numberLabel = UILabel(frame: CGRect.zero)
            charLabel = UILabel(frame: CGRect.zero)
            super.init(frame: frame)
            self.initPropertys()
        }
        
        override func layoutSubviews() {
            super.layoutSubviews()
            self.numberLabel.frame = CGRect(x: 0, y: 0, width: self.bounds.width, height: self.bounds.height - 20)
            self.charLabel.frame = CGRect(x: 0, y: self.numberLabel.bounds.height, width: self.bounds.width, height: 20)
        }
        
        required init?(coder aDecoder: NSCoder) {
            numberLabel = UILabel(frame: CGRect.zero)
            charLabel = UILabel(frame: CGRect.zero)
            super.init(coder: aDecoder)
            self.initPropertys()
        }
        
        func initPropertys() {
            numberLabel.font = UIFont.systemFont(ofSize: 36, weight: .medium)
            numberLabel.textAlignment = .center
            numberLabel.textColor = UIColor.white
            
            charLabel.font = UIFont.systemFont(ofSize: 14)
            charLabel.textAlignment = .center
            charLabel.textColor = UIColor.white
            
            self.addSubview(numberLabel)
            self.addSubview(charLabel)
        }
    }

    private lazy var buttons: [NumberButton] = {
        var array = [NumberButton]()
        for i in 0..<12 {
            
            let button = NumberButton(type: .custom)
            button.addTarget(self, action: #selector(doButtonPressed(_:)), for: .touchUpInside)
            button.tag = i
            button.numberLabel.text = buttonNumList[i]
            button.charLabel.text = buttonCharList[i]
            
            self.addSubview(button)
            array.append(button)
        }
        return array
    }()
    
    private var buttonNumList = ["1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#"]
    private var buttonCharList = ["", "ABC", "DEF", "GHI", "JKL", "MNO", "PQRS", "TUV", "WXYZ", "'", "+", ""]
    
    weak var delegate: DialViewDelegate?
    
    override func layoutSubviews() {
        super.layoutSubviews()
        var rank = 0
        var row = 0
        let rankSpace: CGFloat = 50
        let rankSpaceCount: CGFloat = 2
        let buttonWidth = (self.bounds.width - (rankSpace * rankSpaceCount)) / 3
        
        let buttonHeight: CGFloat = 53
        let rowSpaceCount: CGFloat = 3
        var rowSpace: CGFloat = (self.bounds.height - (buttonHeight * 4)) / rowSpaceCount
        rowSpace = (rowSpace > 50 ? 50 : rowSpace)
        
        for (index, button) in buttons.enumerated() {
            rank = index % 3
            row = index / 3
            
            let x = CGFloat(rank) * (buttonWidth + rankSpace)
            let y = CGFloat(row) * (buttonHeight + rowSpace)
            button.frame = CGRect(x: x,
                                  y: y,
                                  width: buttonWidth,
                                  height: buttonHeight)
        }
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    @objc func doButtonPressed(_ sender: NumberButton) {
        delegate?.dialView(self, didPressedButton: sender.numberLabel.text!)
    }
}
